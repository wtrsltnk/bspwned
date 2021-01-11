#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW_TDM-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/wadloader.o \
	${OBJECTDIR}/textureloader.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/stringcompare.o \
	${OBJECTDIR}/targa.o \
	${OBJECTDIR}/shadermanager.o \
	${OBJECTDIR}/tokenizer.o \
	${OBJECTDIR}/error.o \
	${OBJECTDIR}/worldloader.o \
	${OBJECTDIR}/worldrenderer.o \
	${OBJECTDIR}/vector3.o \
	${OBJECTDIR}/camera.o \
	${OBJECTDIR}/staticrenderer.o \
	${OBJECTDIR}/entitymanager.o \
	${OBJECTDIR}/physics.o \
	${OBJECTDIR}/lightmap.o \
	${OBJECTDIR}/GLee.o \
	${OBJECTDIR}/skyrenderer.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../../../compilers/glsdk_0.4.1/glfw/library -lglfw -lGL -lGLU

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bspwned.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bspwned.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bspwned ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/wadloader.o: wadloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/wadloader.o wadloader.cpp

${OBJECTDIR}/textureloader.o: textureloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/textureloader.o textureloader.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/stringcompare.o: stringcompare.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/stringcompare.o stringcompare.cpp

${OBJECTDIR}/targa.o: targa.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/targa.o targa.cpp

${OBJECTDIR}/shadermanager.o: shadermanager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/shadermanager.o shadermanager.cpp

${OBJECTDIR}/tokenizer.o: tokenizer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/tokenizer.o tokenizer.cpp

${OBJECTDIR}/error.o: error.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/error.o error.cpp

${OBJECTDIR}/worldloader.o: worldloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/worldloader.o worldloader.cpp

${OBJECTDIR}/worldrenderer.o: worldrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/worldrenderer.o worldrenderer.cpp

${OBJECTDIR}/vector3.o: vector3.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/vector3.o vector3.cpp

${OBJECTDIR}/camera.o: camera.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/camera.o camera.cpp

${OBJECTDIR}/staticrenderer.o: staticrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/staticrenderer.o staticrenderer.cpp

${OBJECTDIR}/entitymanager.o: entitymanager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/entitymanager.o entitymanager.cpp

${OBJECTDIR}/physics.o: physics.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/physics.o physics.cpp

${OBJECTDIR}/lightmap.o: lightmap.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/lightmap.o lightmap.cpp

${OBJECTDIR}/GLee.o: GLee.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -g -o ${OBJECTDIR}/GLee.o GLee.c

${OBJECTDIR}/skyrenderer.o: skyrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I../../../../../compilers/glsdk_0.4.1/glfw/include -o ${OBJECTDIR}/skyrenderer.o skyrenderer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bspwned.exe

# Subprojects
.clean-subprojects:
