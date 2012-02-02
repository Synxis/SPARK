# Copy file script without return code (which might mark some build as 'FAILED')
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${DESTINATION})