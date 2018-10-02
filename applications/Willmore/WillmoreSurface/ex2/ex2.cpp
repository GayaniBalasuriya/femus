/** tutorial/Ex3
 * This example shows how to set and solve the weak form of the nonlinear problem
 *                     -\Delta^2 u = f(x) \text{ on }\Omega,
 *            u=0 \text{ on } \Gamma,
 *      \Delat u=0 \text{ on } \Gamma,
 * on a box domain $\Omega$ with boundary $\Gamma$,
 * by using a system of second order partial differential equation.
 * all the coarse-level meshes are removed;
 * a multilevel problem and an equation system are initialized;
 * a direct solver is used to solve the problem.
 **/

#include "FemusInit.hpp"
#include "MultiLevelProblem.hpp"
#include "NumericVector.hpp"
#include "VTKWriter.hpp"
#include "GMVWriter.hpp"
#include "NonLinearImplicitSystem.hpp"
#include "TransientSystem.hpp"
#include "adept.h"
#include <cstdlib>

const unsigned P = 2.;
using namespace femus;


void AssemblePWillmore(MultiLevelProblem& );


int main(int argc, char** args) {

  // init Petsc-MPI communicator
  FemusInit mpinit(argc, args, MPI_COMM_WORLD);


  // define multilevel mesh


  unsigned maxNumberOfMeshes;
 
  MultiLevelMesh mlMsh;
  // read coarse level mesh and generate finers level meshes
  double scalingFactor = 1.;
  
  mlMsh.ReadCoarseMesh("./input/sphere.neu", "seventh", scalingFactor);
  
  unsigned numberOfUniformLevels = 1;
  unsigned numberOfSelectiveLevels = 0;
  mlMsh.RefineMesh(numberOfUniformLevels , numberOfUniformLevels + numberOfSelectiveLevels, NULL);

  // erase all the coarse mesh levels
  // mlMsh.EraseCoarseLevels(numberOfUniformLevels - 1);

  // print mesh info
  mlMsh.PrintInfo();

  // define the multilevel solution and attach the mlMsh object to it
  MultiLevelSolution mlSol(&mlMsh);

    // add variables to mlSol
  mlSol.AddSolution("Dx1", LAGRANGE, SECOND, 0);
  mlSol.AddSolution("Dx2", LAGRANGE, SECOND, 0);
  mlSol.AddSolution("Dx3", LAGRANGE, SECOND, 0);
  mlSol.AddSolution("Y1", LAGRANGE, SECOND, 0);
  mlSol.AddSolution("Y2", LAGRANGE, SECOND, 0);
  mlSol.AddSolution("Y3", LAGRANGE, SECOND, 0);

  mlSol.Initialize("All");
  
  
  MultiLevelProblem mlProb(&mlSol);
  
  // add system Wilmore in mlProb as a Linear Implicit System
  NonLinearImplicitSystem& system = mlProb.add_system < NonLinearImplicitSystem > ("PWillmore");
  
  // add solution "X", "Y", "Z" and "H" to the system
  system.AddSolutionToSystemPDE("Dx1");
  system.AddSolutionToSystemPDE("Dx2");
  system.AddSolutionToSystemPDE("Dx3");
  system.AddSolutionToSystemPDE("Y1");
  system.AddSolutionToSystemPDE("Y2");
  system.AddSolutionToSystemPDE("Y3");
  
  system.SetMaxNumberOfNonLinearIterations(10);
  
  // attach the assembling function to system
  system.SetAssembleFunction(AssemblePWillmore);
  
  // initilaize and solve the system
  system.init();
  
  system.MGsolve();
  
  
  mlSol.SetWriter(VTK);
  std::vector<std::string> mov_vars;
  mov_vars.push_back("Dx1");
  mov_vars.push_back("Dx2");
  mov_vars.push_back("Dx3");  
  mlSol.GetWriter()->SetMovingMesh(mov_vars);
  
  std::vector < std::string > variablesToBePrinted;
  variablesToBePrinted.push_back("All");

  mlSol.GetWriter()->SetDebugOutput(true);
  mlSol.GetWriter()->Write(DEFAULT_OUTPUTDIR, "biquadratic", variablesToBePrinted, 0);
    
  return 0;
}

void AssemblePWillmore(MultiLevelProblem& ml_prob) {
  //  ml_prob is the global object from/to where get/set all the data
  //  level is the level of the PDE system to be assembled
  //  levelMax is the Maximum level of the MultiLevelProblem
  //  assembleMatrix is a flag that tells if only the residual or also the matrix should be assembled
  
  // call the adept stack object
  adept::Stack& s = FemusInit::_adeptStack;
  
  //  extract pointers to the several objects that we are going to use
  NonLinearImplicitSystem* mlPdeSys   = &ml_prob.get_system<NonLinearImplicitSystem> ("PWillmore");   // pointer to the linear implicit system named "Poisson"
  
  const unsigned level = mlPdeSys->GetLevelToAssemble();
  
  Mesh *msh = ml_prob._ml_msh->GetLevel(level);    // pointer to the mesh (level) object
  elem *el = msh->el;  // pointer to the elem object in msh (level)
  
  MultiLevelSolution *mlSol = ml_prob._ml_sol;  // pointer to the multilevel solution object
  Solution *sol = ml_prob._ml_sol->GetSolutionLevel(level);    // pointer to the solution (level) object
  
  LinearEquationSolver *pdeSys = mlPdeSys->_LinSolver[level]; // pointer to the equation (level) object
  SparseMatrix *KK = pdeSys->_KK;  // pointer to the global stifness matrix object in pdeSys (level)
  NumericVector *RES = pdeSys->_RES; // pointer to the global residual vector object in pdeSys (level)
  
  const unsigned  dim = 2; 
  const unsigned  DIM = 3; 
  unsigned iproc = msh->processor_id(); // get the process_id (for parallel computation)
  
  //solution variable
  unsigned solDxIndex[DIM];
  solDxIndex[0] = mlSol->GetIndex("Dx1");  // get the position of "DX" in the ml_sol object
  solDxIndex[1] = mlSol->GetIndex("Dx2");  // get the position of "DY" in the ml_sol object
  solDxIndex[2] = mlSol->GetIndex("Dx3");  // get the position of "DZ" in the ml_sol object
  
  unsigned solxType;
  solxType = mlSol->GetSolutionType(solDxIndex[0]);   // get the finite element type for "U"
    
  unsigned solDxPdeIndex[DIM];
  solDxPdeIndex[0] = mlPdeSys->GetSolPdeIndex("Dx1");    // get the position of "DX" in the pdeSys object
  solDxPdeIndex[1] = mlPdeSys->GetSolPdeIndex("Dx2");    // get the position of "DY" in the pdeSys object
  solDxPdeIndex[2] = mlPdeSys->GetSolPdeIndex("Dx3");    // get the position of "DZ" in the pdeSys object
  
  std::vector < adept::adouble > solx[DIM];  // surface coordinates
  unsigned xType = 2; // get the finite element type for "x", it is always 2 (LAGRANGE QUADRATIC)
   
  unsigned solYIndex[DIM];
  solYIndex[0] = mlSol->GetIndex("Y1");    // get the position of "Y1" in the ml_sol object
  solYIndex[1] = mlSol->GetIndex("Y2");    // get the position of "Y2" in the ml_sol object
  solYIndex[2] = mlSol->GetIndex("Y3");    // get the position of "Y3" in the ml_sol object
  
  unsigned solYType;
  solYType = mlSol->GetSolutionType(solYIndex[0]);   // get the finite element type for "Y"
  
  unsigned solYPdeIndex[DIM];
  solYPdeIndex[0] = mlPdeSys->GetSolPdeIndex("Y1");    // get the position of "Y1" in the pdeSys object
  solYPdeIndex[1] = mlPdeSys->GetSolPdeIndex("Y2");    // get the position of "Y2" in the pdeSys object
  solYPdeIndex[2] = mlPdeSys->GetSolPdeIndex("Y3");    // get the position of "Y3" in the pdeSys object
  
  std::vector < adept::adouble > solY[DIM]; // local Y solution
  std::vector< int > SYSDOF; // local to global pdeSys dofs
 
  vector< double > Res; // local redidual vector
  std::vector< adept::adouble > aResx[3]; // local redidual vector
  std::vector< adept::adouble > aResY[3]; // local redidual vector
    
  vector < double > Jac; // local Jacobian matrix (ordered by column, adept)
     
  KK->zero();  // Set to zero all the entries of the Global Matrix
  RES->zero(); // Set to zero all the entries of the Global Residual
  
  // element loop: each process loops only on the elements that owns
  for (int iel = msh->_elementOffset[iproc]; iel < msh->_elementOffset[iproc + 1]; iel++) {
    
    short unsigned ielGeom = msh->GetElementType(iel);
    unsigned nxDofs  = msh->GetElementDofNumber(iel, solxType);    // number of solution element dofs
    unsigned nYDofs  = msh->GetElementDofNumber(iel, solYType);    // number of solution element dofs
       
    for (unsigned K = 0; K < DIM; K++) {
      solx[K].resize(nxDofs);
      solY[K].resize(nYDofs);
    }
    
    // resize local arrays
    SYSDOF.resize( DIM * ( nxDofs + nYDofs ) );
    
    Res.resize( DIM * ( nxDofs + nYDofs ));    //resize
    
    for (unsigned K = 0; K < DIM; K++) {
      aResx[K].assign(nxDofs,0.);    //resize and set to zero
      aResY[K].assign(nYDofs,0.);    //resize and zet to zero
    }
        
    // local storage of global mapping and solution
    for (unsigned i = 0; i < nxDofs; i++) {
      unsigned iDDof = msh->GetSolutionDof(i, iel, solxType); // global to local mapping between solution node and solution dof
      unsigned iXDof  = msh->GetSolutionDof(i, iel, xType);
      
      for (unsigned K = 0; K < DIM; K++) {
        solx[K][i] = (*msh->_topology->_Sol[K])(iXDof) + (*sol->_Sol[solDxIndex[K]])(iDDof);
        SYSDOF[K * nxDofs + i] = pdeSys->GetSystemDof(solDxIndex[K], solDxPdeIndex[K], i, iel);  // global to global mapping between solution node and pdeSys dof
      }
    }
    
    // local storage of global mapping and solution
    for (unsigned i = 0; i < nYDofs; i++) {
      unsigned iYDof = msh->GetSolutionDof(i, iel, solYType); // global to local mapping between solution node and solution dof
      for (unsigned K = 0; K < DIM; K++) {
        solY[K][i] = (*sol->_Sol[solYIndex[K]])(iYDof);  // global to local solution
        SYSDOF[DIM * nxDofs + K * nYDofs + i] = pdeSys->GetSystemDof(solYIndex[K], solYPdeIndex[K], i, iel);  // global to global mapping between solution node and pdeSys dof
      }
    }
  
    // start a new recording of all the operations involving adept::adouble variables
    s.new_recording();
        
    // *** Gauss point loop ***
    for (unsigned ig = 0; ig < msh->_finiteElement[ielGeom][solxType]->GetGaussPointNumber(); ig++) {
      
      double *phix[DIM];  // local test function
      double *phix_uv[DIM][dim]; // local test function first order partial derivatives
            
      double *phiY[DIM];  // local test function
      double *phiY_uv[DIM][dim]; // local test function first order partial derivatives
      
      double weight; // gauss point weight
            
      // *** get gauss point weight, test function and test function partial derivatives ***
      phix[0] = msh->_finiteElement[ielGeom][solxType]->GetPhi(ig);
      phix_uv[0][0] = msh->_finiteElement[ielGeom][solxType]->GetDPhiDXi(ig); //derivative in u
      phix_uv[0][1] = msh->_finiteElement[ielGeom][solxType]->GetDPhiDEta(ig); //derivative in v
      
      phiY[0] = msh->_finiteElement[ielGeom][solYType]->GetPhi(ig);
      phiY_uv[0][0] = msh->_finiteElement[ielGeom][solYType]->GetDPhiDXi(ig);
      phiY_uv[0][1] = msh->_finiteElement[ielGeom][solYType]->GetDPhiDEta(ig);
      
      for(unsigned K = 1; K < DIM; K++){
        phix[K] = phix[0];
        phiY[K] = phiY[0];
        for(unsigned j = 0; j<dim; j++){
          phix_uv[K][j] = phix_uv[0][j];
          phiY_uv[K][j] = phiY_uv[0][j];
        }
      }
      
      weight = msh->_finiteElement[ielGeom][solxType]->GetGaussWeight(ig);
      
      adept::adouble solx_uv[3][2] = {{0.,0.},{0.,0.},{0.,0.}};
      adept::adouble solY_uv[3][2] = {{0.,0.},{0.,0.},{0.,0.}};
      adept::adouble solYg[3]={0.,0.,0.};
      
      for(unsigned K=1; K < DIM; K++){
        for (unsigned i = 0; i < nxDofs; i++) {
          solYg[K] += phiY[K][i] * solY[K][i];
        }
        for (int j = 0; j < dim; j++) {
          for (unsigned i = 0; i < nxDofs; i++) {
            solx_uv[K][j] += phix_uv[K][j][i] * solx[K][i];
            solY_uv[K][j] += phiY_uv[K][j][i] * solY[K][i];
          }
        }
      }
      
      adept::adouble solYnorm2 = 0.;
      for(unsigned K=1; K < DIM; K++){
        solYnorm2 += solYg[K] * solYg[K];
      }
            
      adept::adouble g[dim][dim]={{0.,0.},{0.,0.}};
      for(unsigned i=0; i < dim; i++){
        for(unsigned j=0; j < dim; j++){
          for(unsigned K=0; K < DIM; K++){
            g[i][j] += solx_uv[K][i] * solx_uv[K][j];
          }
        }
      }
      adept::adouble detg = g[0][0] * g[1][1] - g[0][1] * g[1][0];
      adept::adouble gi[dim][dim];
      gi[0][0] =  g[1][1]/detg;
      gi[0][1] = -g[0][1]/detg;
      gi[1][0] = -g[1][0]/detg;
      gi[1][1] =  g[0][0]/detg;
      
      adept::adouble Area = weight * sqrt(detg);
      
      adept::adouble Jir[2][3]={{0.,0.,0.},{0.,0.,0.}};
      for(unsigned i = 0; i < dim; i++){
        for(unsigned J = 0; J < DIM; J++){
          for(unsigned k = 0; k < dim; k++){
            Jir[i][J] += gi[i][k] * solx_uv[J][k];
          }
        }
      }
      
      adept::adouble solx_Xtan[DIM][DIM]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
      adept::adouble solY_Xtan[DIM][DIM]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
      std::vector < adept::adouble > phiY_Xtan[DIM][DIM];
      std::vector < adept::adouble > phix_Xtan[DIM][DIM];
      
      for(unsigned I = 0; I < DIM; I++){
        for(unsigned J = 0; J < DIM; J++){
          for(unsigned k = 0; k < dim; k++){
            solx_Xtan[I][J] += solx_uv[I][k] * Jir[k][J];
            solY_Xtan[I][J] += solY_uv[I][k] * Jir[k][J];
          }
          phiY_Xtan[I][J].assign(nYDofs,0.);
          phix_Xtan[I][J].assign(nYDofs,0.);
          for(unsigned inode  = 0; inode < nYDofs; inode++){
            for(unsigned k = 0; k < dim; k++){
              phiY_Xtan[I][J][inode] += phiY_uv[I][k][inode] * Jir[k][J];
              phix_Xtan[I][J][inode] += phix_uv[I][k][inode] * Jir[k][J];
            }
          }
        }
      }
      
      for(unsigned K = 0; K < DIM; K++){
        for(unsigned i = 0; i < nxDofs; i++){
          aResx[K][i] += (solYg[K] * phix[K][i] + phix_Xtan[K][K][i]) * Area; 
        }
        for(unsigned i = 0; i < nYDofs; i++){
          adept::adouble term1 = (1. - 2. * P) * solYnorm2;
          adept::adouble term2 = 0.;
          adept::adouble term3 = 0.;
          for(unsigned J = 0; J < DIM; J++){
            term1 -= P * solY_Xtan[J][J];
            term2 +=  solY_Xtan[K][J] * phiY_Xtan[K][J][i]; 
            adept::adouble term4 = 0.;
            for(unsigned L = 0; L < DIM; L++){
              term4 += solx_Xtan[J][L] * solY_Xtan[K][L] + solx_Xtan[K][L] * solY_Xtan[J][L];
            }
            term3 += phiY_Xtan[K][J][i] * term4;
          }
          aResY[K][i] += (term1 * phiY_Xtan[K][K][i] - P * term2 + P * term3) * Area; 
        }
      }
      
     /* 
      
      // evaluate the solution, the solution derivatives and the coordinates in the gauss point
      adept::adouble solRGauss[3];
      double solRGaussOld[3];
      adept::adouble solRGauss_x[3][2];
      adept::adouble solRGauss_xx[3][2][2];
      
      adept::adouble sol_x[2];
      sol_x[0] = sol_x[1] = 0.;
      
      for (int k = 0; k < 3; k++) {
        solRGauss[k] = 0.;
        solRGaussOld[k] = 0.;
        
        for (int i = 0; i < dim; i++) {
          solRGauss_x[k][i] = 0.;
          
          for (int j = 0; j < dim; j++) {
            solRGauss_xx[k][i][j] = 0.;
          }
        }
      }
      
      adept::adouble solHGauss = 0;
      adept::adouble solHGauss_x[2] = {0., 0.};
      
      for (unsigned i = 0; i < nDofs; i++) {
        for (int k = 0; k < 3; k++) {
          solRGauss[k] += phi[i] * solR[k][i];
          solRGaussOld[k] += phi[i] * solR_old[k][i];
        }
        
        solHGauss += phi[i] * solH[i];
        
        for (unsigned u = 0; u < dim; u++) {
          sol_x[u] += phi[i] * x[u][i];
        }
        
        for (unsigned u = 0; u < dim; u++) { // gradient
          for (int k = 0; k < 3; k++) {
            solRGauss_x[k][u] += phi_x[i * dim + u] * solR[k][i];
          }
          
          solHGauss_x[u] += phi_x[i * dim + u] * solH[i];
        }
        
        for (unsigned u = 0; u < dim; u++) {  // hessian
          for (unsigned v = 0; v < dim; v++) {
            unsigned uvindex = 0; //_uu
            
            if (u != v) uvindex = 2;  //_uv or _vu
            else if (u == 1) uvindex = 1;  //_vv
            
            for (int k = 0; k < 3; k++) {
              solRGauss_xx[k][u][v] += phi_xx[i * dim2 + uvindex] * solR[k][i];
            }
          }
        }
      }
      
      adept::adouble g[2][2];
      
      
      g[0][0] = g[0][1] = g[1][0] = g[1][1] = 0.;
      
      for (int k = 0; k < 3; k++) {
        for (int u = 0; u < dim; u++) {
          for (int v = 0; v < dim; v++) {
            g[u][v] += solRGauss_x[k][u] * solRGauss_x[k][v];
          }
        }
      }
      
      adept::adouble detg = g[0][0] * g[1][1] - g[0][1] * g[1][0];
      
      adept::adouble  A = sqrt(detg);
      
      adept::adouble gI[2][2];
      
      gI[0][0] =  g[1][1] / detg;
      gI[0][1] = -g[0][1] / detg;
      gI[1][0] = -g[1][0] / detg;
      gI[1][1] =  g[0][0] / detg;
      
      adept::adouble N[3];
      
      N[0] = (solRGauss_x[1][0] * solRGauss_x[2][1] - solRGauss_x[1][1] * solRGauss_x[2][0]) / A;
      N[1] = (solRGauss_x[2][0] * solRGauss_x[0][1] - solRGauss_x[2][1] * solRGauss_x[0][0]) / A;
      N[2] = (solRGauss_x[0][0] * solRGauss_x[1][1] - solRGauss_x[0][1] * solRGauss_x[1][0]) / A;
      
      adept::adouble h[2][2];
      
      h[0][0] = h[0][1] = h[1][0] = h[1][1] = 0.;
      
      for (int k = 0; k < 3; k++) {
        for (int u = 0; u < dim; u++) {
          for (int v = 0; v < dim; v++) {
            h[u][v] += solRGauss_xx[k][u][v] * N[k];
            
          }
        }
      }
      
      //adept::adouble K = cos(sol_x[0])/(a+cos(sol_x[0]));//(h[0][0]*h[1][1]-h[0][1]*h[1][0])/detg;
      
      adept::adouble K = (h[0][0] * h[1][1] - h[0][1] * h[1][0]) / detg;
      
      adept::adouble H_exact = 1;
      
      // *** phi_i loop ***
      for (unsigned i = 0; i < nDofs; i++) {
        
        for (int k = 0; k < 3; k++) {
          for (int u = 0; u < dim; u++) {
            adept::adouble AgIgradRgradPhi = 0;
            
            for (int v = 0; v < dim; v++) {
              AgIgradRgradPhi += A * gI[u][v].value() * solRGauss_x[k][v];
            }
            
            aResR[k][i] += AgIgradRgradPhi * phi_x[i * dim + u] * weight;
          }
          
          aResR[k][i] += 2.* A * solHGauss.value() * N[k] * phi[i] * weight;
        }
        
        
        for (int u = 0; u < dim; u++) {
          adept::adouble AgIgradHgradPhi = 0;
          
          for (int v = 0; v < dim; v++) {
            AgIgradHgradPhi += A * gI[u][v].value() * solHGauss_x[v];
          }
          
          aResH[i] -= AgIgradHgradPhi * phi_x[i * dim + u] * weight;
        }
        
        aResH[i] += A * (-0 * (solRGauss[0] - solRGaussOld[0]) * N[0].value()
        - 0 * (solRGauss[1] - solRGaussOld[1]) * N[1].value()
        - 0 * (solRGauss[2] - solRGaussOld[2]) * N[2].value()
        + 2. * solHGauss * (solHGauss * solHGauss  - K.value())) * phi[i] * weight;
        
      } // end phi_i loop*/
     
    } // end gauss point loop
    
    //--------------------------------------------------------------------------------------------------------
    // Add the local Matrix/Vector into the global Matrix/Vector
    
    //copy the value of the adept::adoube aRes in double Res and store
    
    for (int K = 0; K < DIM; K++) {
      for (int i = 0; i < nxDofs; i++) {
        Res[ K * nxDofs + i] = -aResx[K][i].value();
      }
      for (int i = 0; i < nYDofs; i++) {
        Res[DIM * nxDofs + K * nYDofs + i] = -aResY[K][i].value();
      }
    }
    
    RES->add_vector_blocked(Res, SYSDOF);
    
    
    Jac.resize(DIM * (nxDofs + nYDofs) * DIM * (nxDofs + nYDofs) );
    
    // define the dependent variables
    for (int K = 0; K < DIM; K++) {
      s.dependent(&aResx[K][0], nxDofs);
    }
    for (int K = 0; K < DIM; K++) {
      s.dependent(&aResY[K][0], nYDofs);
    }
     
    // define the dependent variables
    for (int K = 0; K < DIM; K++) {
      s.independent(&solx[K][0], nxDofs);
    }
    for (int K = 0; K < DIM; K++) {
      s.independent(&solY[K][0], nYDofs);
    } 
        
    // get the jacobian matrix (ordered by row)
    s.jacobian(&Jac[0], true);
    
    KK->add_matrix_blocked(Jac, SYSDOF, SYSDOF);
    
    s.clear_independents();
    s.clear_dependents();
    
  } //end element loop for each process
  
  RES->close();
  
  KK->close();
  
  // ***************** END ASSEMBLY *******************
}

