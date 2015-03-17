# Install script for directory: /home/ASP2/rawtherapee/rtdata

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/home/ASP2/rawtherapee/build/debug")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/applications/rawtherapee.desktop")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/applications" TYPE FILE FILES "/home/ASP2/rawtherapee/build/rtdata/rawtherapee.desktop")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/16x16/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/16x16/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi16-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/24x24/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/24x24/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi24-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/32x32/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/32x32/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi32-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/48x48/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/48x48/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi48-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/128x128/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/128x128/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi128-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/256x256/apps/rawtherapee.png")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/home/ASP2/rawtherapee/build/debug/share/icons/hicolor/256x256/apps" TYPE FILE RENAME "rawtherapee.png" FILES "/home/ASP2/rawtherapee/rtdata/icons/hi256-app-rawtherapee.png")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./languages" TYPE FILE FILES
    "/home/ASP2/rawtherapee/rtdata/languages/Norsk BM"
    "/home/ASP2/rawtherapee/rtdata/languages/Chinese (Simplified)"
    "/home/ASP2/rawtherapee/rtdata/languages/Deutsch"
    "/home/ASP2/rawtherapee/rtdata/languages/Portugues (Brasil)"
    "/home/ASP2/rawtherapee/rtdata/languages/English (US)"
    "/home/ASP2/rawtherapee/rtdata/languages/Serbian (Latin Characters)"
    "/home/ASP2/rawtherapee/rtdata/languages/LICENSE"
    "/home/ASP2/rawtherapee/rtdata/languages/Francais"
    "/home/ASP2/rawtherapee/rtdata/languages/Polish"
    "/home/ASP2/rawtherapee/rtdata/languages/README"
    "/home/ASP2/rawtherapee/rtdata/languages/Suomi"
    "/home/ASP2/rawtherapee/rtdata/languages/Dansk"
    "/home/ASP2/rawtherapee/rtdata/languages/Catala"
    "/home/ASP2/rawtherapee/rtdata/languages/default"
    "/home/ASP2/rawtherapee/rtdata/languages/Turkish"
    "/home/ASP2/rawtherapee/rtdata/languages/Espanol"
    "/home/ASP2/rawtherapee/rtdata/languages/Latvian"
    "/home/ASP2/rawtherapee/rtdata/languages/Russian"
    "/home/ASP2/rawtherapee/rtdata/languages/Japanese"
    "/home/ASP2/rawtherapee/rtdata/languages/Euskara"
    "/home/ASP2/rawtherapee/rtdata/languages/Magyar"
    "/home/ASP2/rawtherapee/rtdata/languages/Italiano"
    "/home/ASP2/rawtherapee/rtdata/languages/Hebrew"
    "/home/ASP2/rawtherapee/rtdata/languages/Serbian (Cyrilic Characters)"
    "/home/ASP2/rawtherapee/rtdata/languages/Slovak"
    "/home/ASP2/rawtherapee/rtdata/languages/Czech"
    "/home/ASP2/rawtherapee/rtdata/languages/Greek"
    "/home/ASP2/rawtherapee/rtdata/languages/Nederlands"
    "/home/ASP2/rawtherapee/rtdata/languages/Chinese (Traditional)"
    "/home/ASP2/rawtherapee/rtdata/languages/Polish (Latin Characters)"
    "/home/ASP2/rawtherapee/rtdata/languages/Swedish"
    "/home/ASP2/rawtherapee/rtdata/languages/English (UK)"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./sounds" TYPE FILE FILES
    "/home/ASP2/rawtherapee/rtdata/sounds/ProcessComplete.wav"
    "/home/ASP2/rawtherapee/rtdata/sounds/BatchComplete.wav"
    "/home/ASP2/rawtherapee/rtdata/sounds/Empty.wav"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./iccprofiles/input" TYPE FILE FILES
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Sony SLT-A55V.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS 550D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D700.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-FZ150.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D3000.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-FZ35.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D200.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon PowerShot G10.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-GH1.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-FZ38.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS 450D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Sony DSLR-A700.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-G1.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS 5D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Pentax K200D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D3S.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D7000.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon PowerShot G12.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Olympus E-P2.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-GH2.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Sony DSLR-A900.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Panasonic DMC-G3.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS Digital Rebel XSi.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS-1D Mark III.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS Rebel T2i.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS 40D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Canon EOS 20D.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/input/Nikon D3100.icc"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./iccprofiles/output" TYPE FILE FILES
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_sRGB_g10.icm"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_Large_gsRGB.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_Large_gBT709.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_sRGB_gBT709.icm"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_Medium_gsRGB.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_Large_g10.icc"
    "/home/ASP2/rawtherapee/rtdata/iccprofiles/output/RT_sRGB.icm"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./dcpprofiles" TYPE FILE FILES
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Pentax K10D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus XZ-1.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus E-P2.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D7000.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D200.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon PowerShot S110.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Pentax K-5.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Fujifilm FinePix S9500.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-FZ35.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Sony NEX-5N.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS D60.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D750.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-GH1.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-GH2.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus E-M5.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-G3.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus E-520.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 400D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Pentax K200D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-G1.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Sony SLT-A55V.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS Rebel T2i.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus E-1.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS-1D Mark III.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D50.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Sony DSLR-A900.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-FZ38.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D800.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon PowerShot G10.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Pentax K-r.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 5D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Pentax K-5 II.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D700.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 5D Mark III.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Sony DSLR-A700.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D3S.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 550D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS Digital Rebel XTi.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 7D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Olympus E-510.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-G5.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D3000.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon PowerShot G12.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Leaf Aptus 75.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D300.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Panasonic DMC-FZ150.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D3100.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 20D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS Digital Rebel XSi.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Nikon D5100.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 40D.dcp"
    "/home/ASP2/rawtherapee/rtdata/dcpprofiles/Canon EOS 450D.dcp"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "/home/ASP2/rawtherapee/rtdata/profiles" FILES_MATCHING REGEX "/[^/]*\\.pp3$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "/home/ASP2/rawtherapee/rtdata/themes")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "/home/ASP2/rawtherapee/rtdata/images" FILES_MATCHING REGEX "/index\\.theme$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "/home/ASP2/rawtherapee/rtdata/images" FILES_MATCHING REGEX "/[^/]*\\.png$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ RENAME "options" FILES "/home/ASP2/rawtherapee/rtdata/options/options.lin")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

