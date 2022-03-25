#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.7.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()
sys.path.insert(0, r'/home/student/software/femus/applications/2021_fall/GayaniBalasuriya/input')

####################################################
##       Begin of NoteBook variables section      ##
####################################################
notebook.set("r", 1)
####################################################
##        End of NoteBook variables section       ##
####################################################
###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New()

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Divided_Disk_1 = geompy.MakeDividedDisk("r", 1, GEOM.SQUARE)
geompy.Rotate(Divided_Disk_1, OZ, 45*math.pi/180.0)
[Face_1,Face_5,Edge_1,Edge_2,Edge_3,Edge_4,Edge_5,Edge_6,Edge_7,Face_2,Face_3,Face_4] = geompy.ExtractShapes(Divided_Disk_1, geompy.ShapeType["EDGE"], True)
[Face_1,Face_2,Face_3,Face_4,Face_5] = geompy.ExtractShapes(Divided_Disk_1, geompy.ShapeType["FACE"], True)
[Edge_1,Edge_2,Edge_3,Edge_4] = geompy.ExtractShapes(Face_1, geompy.ShapeType["EDGE"], True)
[Edge_5,Edge_6,Edge_7,Edge_8] = geompy.ExtractShapes(Face_2, geompy.ShapeType["EDGE"], True)
[Edge_9,Edge_10,Edge_11,Edge_12] = geompy.ExtractShapes(Face_3, geompy.ShapeType["EDGE"], True)
[Edge_13,Edge_14,Edge_15,Edge_16] = geompy.ExtractShapes(Face_4, geompy.ShapeType["EDGE"], True)
[Face_1, Face_2, Face_3, Face_4, Face_5] = geompy.GetExistingSubObjects(Divided_Disk_1, False)
[Edge_17,Edge_18,Edge_19,Edge_20] = geompy.ExtractShapes(Face_5, geompy.ShapeType["EDGE"], True)
Auto_group_for_Group_1 = geompy.CreateGroup(Divided_Disk_1, geompy.ShapeType["EDGE"])
geompy.UnionList(Auto_group_for_Group_1, [Edge_1, Edge_6, Edge_15, Edge_20])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Divided_Disk_1, 'Divided Disk_1' )
geompy.addToStudyInFather( Divided_Disk_1, Face_1, 'Face_1' )
geompy.addToStudyInFather( Divided_Disk_1, Face_5, 'Face_5' )
geompy.addToStudyInFather( Face_1, Edge_1, 'Edge_1' )
geompy.addToStudyInFather( Face_1, Edge_2, 'Edge_2' )
geompy.addToStudyInFather( Face_1, Edge_3, 'Edge_3' )
geompy.addToStudyInFather( Face_1, Edge_4, 'Edge_4' )
geompy.addToStudyInFather( Face_2, Edge_5, 'Edge_5' )
geompy.addToStudyInFather( Face_2, Edge_6, 'Edge_6' )
geompy.addToStudyInFather( Face_2, Edge_7, 'Edge_7' )
geompy.addToStudyInFather( Divided_Disk_1, Face_2, 'Face_2' )
geompy.addToStudyInFather( Divided_Disk_1, Face_3, 'Face_3' )
geompy.addToStudyInFather( Divided_Disk_1, Face_4, 'Face_4' )
geompy.addToStudyInFather( Face_2, Edge_8, 'Edge_8' )
geompy.addToStudyInFather( Face_3, Edge_9, 'Edge_9' )
geompy.addToStudyInFather( Face_3, Edge_10, 'Edge_10' )
geompy.addToStudyInFather( Face_3, Edge_11, 'Edge_11' )
geompy.addToStudyInFather( Face_3, Edge_12, 'Edge_12' )
geompy.addToStudyInFather( Face_4, Edge_13, 'Edge_13' )
geompy.addToStudyInFather( Face_4, Edge_14, 'Edge_14' )
geompy.addToStudyInFather( Face_4, Edge_15, 'Edge_15' )
geompy.addToStudyInFather( Face_4, Edge_16, 'Edge_16' )
geompy.addToStudyInFather( Face_5, Edge_17, 'Edge_17' )
geompy.addToStudyInFather( Face_5, Edge_18, 'Edge_18' )
geompy.addToStudyInFather( Face_5, Edge_19, 'Edge_19' )
geompy.addToStudyInFather( Face_5, Edge_20, 'Edge_20' )
geompy.addToStudyInFather( Divided_Disk_1, Auto_group_for_Group_1, 'Auto_group_for_Group_1' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New()
#smesh.SetEnablePublish( False ) # Set to False to avoid publish in study if not needed or in some particular situations:
                                 # multiples meshes built in parallel, complex and numerous mesh edition (performance)

Mesh_1 = smesh.Mesh(Divided_Disk_1)
Regular_1D = Mesh_1.Segment(geom=Edge_1)
Number_of_Segments_1 = Regular_1D.NumberOfSegments(3)
Propagation_of_1D_Hyp = Regular_1D.Propagation()
Regular_1D_1 = Mesh_1.Segment(geom=Edge_2)
Number_of_Segments_2 = Regular_1D_1.NumberOfSegments(3)
status = Mesh_1.AddHypothesis(Propagation_of_1D_Hyp,Edge_2)
Number_of_Segments_3 = smesh.CreateHypothesis('NumberOfSegments')
Number_of_Segments_3.SetNumberOfSegments( 3 )
Propagation_of_1D_Hyp_1 = smesh.CreateHypothesis('Propagation')
Propagation_of_1D_Hyp_2 = smesh.CreateHypothesis('Propagation')
#Mesh_1.GetMesh().RemoveSubMesh( smeshObj_1 ) ### smeshObj_1 has not been yet created
Regular_1D_2 = Mesh_1.Segment(geom=Edge_2)
Number_of_Segments_4 = Regular_1D_1.NumberOfSegments(3)
Propagation_of_1D_Hyp_3 = Regular_1D_1.Propagation()
Regular_1D_3 = Mesh_1.Segment(geom=Edge_6)
Number_of_Segments_5 = Regular_1D_3.NumberOfSegments(3)
Propagation_of_1D_Hyp_4 = Regular_1D_3.Propagation()
Regular_1D_4 = Mesh_1.Segment()
Quadrangle_2D = Mesh_1.Quadrangle(algo=smeshBuilder.QUADRANGLE)
Group_1_0 = Mesh_1.GroupOnGeom(Auto_group_for_Group_1,'Group_1',SMESH.EDGE)
[ Group_1_0 ] = Mesh_1.GetGroups()
isDone = Mesh_1.Compute()
[ Group_1_0 ] = Mesh_1.GetGroups()
smesh.SetName(Mesh_1, 'Mesh_1')
try:
  Mesh_1.ExportMED(r'/home/student/software/femus/applications/2021_fall/GayaniBalasuriya/input/assignmentDiskQuad.med',auto_groups=0,version=41,overwrite=1,meshPart=None,autoDimension=0)
  pass
except:
  print('ExportMED() failed. Invalid file name?')
aCriteria = []
aCriterion = smesh.GetCriterion(SMESH.EDGE,SMESH.FT_FreeBorders,SMESH.FT_Undefined,0,SMESH.FT_LogicalNOT)
aCriteria.append(aCriterion)
isDone = Mesh_1.RemoveElements( [ 1, 2, 3, 7, 8, 9, 10, 11, 12, 16, 17, 18, 19, 20, 21, 25, 26, 27, 28, 29, 30, 34, 35, 36 ] )
Group_1_0.SetName( 'Group_1_0' )
smesh.SetName(Mesh_1, 'Mesh_1')
try:
  Mesh_1.ExportMED(r'/home/student/software/femus/applications/2021_fall/GayaniBalasuriya/input/assignmentDiskQuad.med',auto_groups=0,version=41,overwrite=1,meshPart=None,autoDimension=0)
  pass
except:
  print('ExportMED() failed. Invalid file name?')
Sub_mesh_1 = Regular_1D.GetSubMesh()
Sub_mesh_2 = Regular_1D_1.GetSubMesh()
Sub_mesh_3 = Regular_1D_3.GetSubMesh()


## Set names of Mesh objects
smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
smesh.SetName(Quadrangle_2D.GetAlgorithm(), 'Quadrangle_2D')
smesh.SetName(Propagation_of_1D_Hyp, 'Propagation of 1D Hyp. on Opposite Edges_1')
smesh.SetName(Number_of_Segments_2, 'Number of Segments_2')
smesh.SetName(Number_of_Segments_1, 'Number of Segments_1')
smesh.SetName(Propagation_of_1D_Hyp_2, 'Propagation of 1D Hyp. on Opposite Edges_3')
smesh.SetName(Number_of_Segments_4, 'Number of Segments_4')
smesh.SetName(Number_of_Segments_3, 'Number of Segments_3')
smesh.SetName(Propagation_of_1D_Hyp_1, 'Propagation of 1D Hyp. on Opposite Edges_2')
smesh.SetName(Propagation_of_1D_Hyp_3, 'Propagation of 1D Hyp. on Opposite Edges_4')
smesh.SetName(Number_of_Segments_5, 'Number of Segments_5')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')
smesh.SetName(Propagation_of_1D_Hyp_4, 'Propagation of 1D Hyp. on Opposite Edges_5')
smesh.SetName(Group_1_0, 'Group_1_0')
smesh.SetName(Sub_mesh_3, 'Sub-mesh_3')
smesh.SetName(Sub_mesh_2, 'Sub-mesh_2')
smesh.SetName(Sub_mesh_1, 'Sub-mesh_1')


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
