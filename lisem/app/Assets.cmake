
#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedKernels "")
 foreach(QmlFile ${kernel_src})
   get_filename_component(nam ${QmlFile} NAME)
   configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/release/kernels/${nam} COPYONLY)
   list(APPEND copiedKernels ${CMAKE_CURRENT_BINARY_DIR}/release/kernels/${nam})
 endforeach()

#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedResources "")
 foreach(QmlFile ${resources})
    get_filename_component(nam ${QmlFile} NAME)
	configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/release/assets/${nam} COPYONLY)
  
   list(APPEND copiedResources ${CMAKE_CURRENT_BINARY_DIR}/release/assets/${nam})
 endforeach()

#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedKernels "")
 foreach(QmlFile ${kernel_src})
   get_filename_component(nam ${QmlFile} NAME)
   
   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/release/kernels/${nam}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/release/kernels/
     DEPENDS ${QmlFile}
     COMMENT "Copying ${QmlFile}"
     VERBATIM
   )
   list(APPEND copiedKernels ${CMAKE_CURRENT_BINARY_DIR}/release/kernels/${nam})
 endforeach()

add_library(Kernels SHARED ${kernel_src} ${copiedKernels})

#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedResources "")
 foreach(QmlFile ${resources})
    get_filename_component(nam ${QmlFile} NAME)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/release/assets/${nam}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/release/assets/
     DEPENDS ${QmlFile}
     COMMENT "Copying ${QmlFile}"
     VERBATIM
   )
   list(APPEND copiedResources ${CMAKE_CURRENT_BINARY_DIR}/release/assets/${nam})
 endforeach()

add_library(Resources SHARED ${resources} ${copiedResources})


