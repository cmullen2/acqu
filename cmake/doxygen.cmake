# look for Doxygen
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
  return()
endif()

# In DAQ mode, we create only the core documentation
# else, everything is scanned by Doxygen
if(ACQU_DAQ)
  set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}/acqu_core")
else()
  set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")
endif()

# since we could some Qt documentation
# we check for that, don't know if Qt3 works as well
find_package(Qt4)
if(QT_FOUND)
  set(DOXYGEN_MAKE_QTHELP "YES")
  find_program(QT_HELPGENERATOR_EXE qhelpgenerator ${QT_BINARY_DIR})
  find_program(QT_ASSISTANT_EXE assistant-qt4 ${QT_BINARY_DIR})
  set(DOXYGEN_QCH_FILE ${CMAKE_BINARY_DIR}/doc/qthelp.qch)
  set(DOXYGEN_QHP_FILE ${CMAKE_BINARY_DIR}/doc/html/index.qhp)  
  # if we're not Linux, we cannot patch the QHP file
  # otherwise we do the complicated business below
  if(NOT CMAKE_SYSTEM_NAME MATCHES Linux)
    # let doxygen directly create the QCH file
    message(STATUS "QtCreator help might miss images") 
    set(DOXYGEN_QCH_GEN ${QT_HELPGENERATOR_EXE})
  endif()
endif()

# we use one global Doxyfile
configure_file(${CMAKE_SOURCE_DIR}/cmake/Doxyfile.in
  ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

# and define how to run Doxygen
# it always outputs a symbolic doc dir to avoid clashing
# with the target "doc"
add_custom_command(OUTPUT "DoxyfileDocDir"
  COMMAND ${DOXYGEN_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Running Doxygen...")
set_source_files_properties("DoxyfileDocDir" PROPERTIES SYMBOLIC "on")

# no Qt or not on Linux, that's easy
if(NOT QT_FOUND OR NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
  # so we can make it saying "make doc"
  add_custom_target(doc DEPENDS "DoxyfileDocDir")
  return()
endif()


# do some more stuff if Qt is there and we're on Linux
# generate the patched qtcreator help file
# and auto-register the help file

# patch the QHP file, Doxygen seems to be
# guilty for this ugly patch!
add_custom_command(OUTPUT ${DOXYGEN_QHP_FILE}
  COMMAND
  ${CMAKE_SOURCE_DIR}/cmake/patch-doxygen-qhp-file.sh  ${DOXYGEN_QHP_FILE} 
  DEPENDS "DoxyfileDocDir"
  COMMENT "Patching Doxygen's QHP file...")

# manually compile QHP to QCH file
add_custom_command(OUTPUT ${DOXYGEN_QCH_FILE}
  COMMAND
  ${QT_HELPGENERATOR_EXE} -o ${DOXYGEN_QCH_FILE} ${DOXYGEN_QHP_FILE} > /dev/null 
  DEPENDS ${DOXYGEN_QHP_FILE}
  COMMENT "Generating Qt help...")


# try to auto register it
# qtcreator should have been started already for that
find_file(QTCREATOR_HELP_FILE helpcollection.qhc $ENV{HOME}/.config/Nokia/qtcreator)
if(QTCREATOR_HELP_FILE)
  message(STATUS "Found QtCreator, will auto-register help file")
  # unregister first to force update...ignore errors
  # and register it
  add_custom_command(OUTPUT "QtAssistantRegistration"
    COMMAND ${QT_ASSISTANT_EXE} -unregister ${DOXYGEN_QCH_FILE}
    -collectionFile ${QTCREATOR_HELP_FILE} 2> /dev/null >/dev/null || exit 0
    COMMAND
    ${QT_ASSISTANT_EXE} -register ${DOXYGEN_QCH_FILE}
    -collectionFile ${QTCREATOR_HELP_FILE} > /dev/null
    DEPENDS ${DOXYGEN_QCH_FILE} 
    COMMENT "Registering Qt help...") 
  set_source_files_properties("QtAssistantRegistration" PROPERTIES SYMBOLIC "on")
  # finally we can make it saying "make doc"
  add_custom_target(doc DEPENDS "QtAssistantRegistration")
else()
  # finally we can make it saying "make doc"
  add_custom_target(doc DEPENDS ${DOXYGEN_QCH_FILE})
endif()






