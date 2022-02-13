#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.7.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()
sys.path.insert(0, r'/home/student/software/femus/applications/2021_fall/hdongamm/input')

####################################################
##       Begin of NoteBook variables section      ##
####################################################
notebook.set("l_x", 100)
notebook.set("l_y", 100)
notebook.set("l_x_n", "l_x*2")
notebook.set("l_y_n", "l_y*(-0.5)")
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
geomObj_1 = geompy.MakeDiskR(100, 1)
geomObj_2 = geompy.MakeFaceHW(200, 200, 1)
Face_1 = geompy.MakeFaceHW(100, 100, 1)
Translation_1 = geompy.MakeTranslation(Face_1, 0, "l_y_n", 0)
Disk_1 = geompy.MakeDiskR(50, 1)
Cut_1 = geompy.MakeCutList(Disk_1, [Translation_1], True)
[Edge_1,Edge_2] = geompy.ExtractShapes(Cut_1, geompy.ShapeType["EDGE"], True)
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Translation_1, 'Translation_1' )
geompy.addToStudy( Disk_1, 'Disk_1' )
geompy.addToStudy( Cut_1, 'Cut_1' )
geompy.addToStudyInFather( Cut_1, Edge_1, 'Edge_1' )
geompy.addToStudyInFather( Cut_1, Edge_2, 'Edge_2' )


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
