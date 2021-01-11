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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/shadermanager.o \
	${OBJECTDIR}/tokenizer.o \
	${OBJECTDIR}/staticrenderer.o \
	${OBJECTDIR}/entitymanager.o \
	${OBJECTDIR}/wadloader.o \
	${OBJECTDIR}/lightmap.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/camera.o \
	${OBJECTDIR}/physics.o \
	${OBJECTDIR}/error.o \
	${OBJECTDIR}/skyrenderer.o \
	${OBJECTDIR}/worldloader.o \
	${OBJECTDIR}/textureloader.o \
	${OBJECTDIR}/worldrenderer.o \
	${OBJECTDIR}/targa.o \
	${OBJECTDIR}/vector3.o \
	${OBJECTDIR}/stringcompare.o \
	${OBJECTDIR}/GLee.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/MinGW-Windows/bspwned.exe

dist/Release/MinGW-Windows/bspwned.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Release/MinGW-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bspwned ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/shadermanager.o: nbproject/Makefile-${CND_CONF}.mk shadermanager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/shadermanager.o shadermanager.cpp

${OBJECTDIR}/tokenizer.o: nbproject/Makefile-${CND_CONF}.mk tokenizer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/tokenizer.o tokenizer.cpp

${OBJECTDIR}/staticrenderer.o: nbproject/Makefile-${CND_CONF}.mk staticrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/staticrenderer.o staticrenderer.cpp

${OBJECTDIR}/entitymanager.o: nbproject/Makefile-${CND_CONF}.mk entitymanager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/entitymanager.o entitymanager.cpp

${OBJECTDIR}/wadloader.o: nbproject/Makefile-${CND_CONF}.mk wadloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/wadloader.o wadloader.cpp

${OBJECTDIR}/lightmap.o: nbproject/Makefile-${CND_CONF}.mk lightmap.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/lightmap.o lightmap.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/camera.o: nbproject/Makefile-${CND_CONF}.mk camera.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/camera.o camera.cpp

${OBJECTDIR}/physics.o: nbproject/Makefile-${CND_CONF}.mk physics.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/physics.o physics.cpp

${OBJECTDIR}/error.o: nbproject/Makefile-${CND_CONF}.mk error.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/error.o error.cpp

${OBJECTDIR}/skyrenderer.o: nbproject/Makefile-${CND_CONF}.mk skyrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/skyrenderer.o skyrenderer.cpp

${OBJECTDIR}/worldloader.o: nbproject/Makefile-${CND_CONF}.mk worldloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/worldloader.o worldloader.cpp

${OBJECTDIR}/textureloader.o: nbproject/Makefile-${CND_CONF}.mk textureloader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/textureloader.o textureloader.cpp

${OBJECTDIR}/worldrenderer.o: nbproject/Makefile-${CND_CONF}.mk worldrenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/worldrenderer.o worldrenderer.cpp

${OBJECTDIR}/targa.o: nbproject/Makefile-${CND_CONF}.mk targa.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/targa.o targa.cpp

${OBJECTDIR}/vector3.o: nbproject/Makefile-${CND_CONF}.mk vector3.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/vector3.o vector3.cpp

${OBJECTDIR}/stringcompare.o: nbproject/Makefile-${CND_CONF}.mk stringcompare.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/stringcompare.o stringcompare.cpp

${OBJECTDIR}/GLee.o: nbproject/Makefile-${CND_CONF}.mk GLee.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -o ${OBJECTDIR}/GLee.o GLee.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/MinGW-Windows/bspwned.exe

# Subprojects
.clean-subprojects:
