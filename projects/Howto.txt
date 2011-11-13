SPARK Build guide
-----------------

Note
	${SPARK_DIR} refers to the folder where SPARK is installed.

Source directories:

	- For the engine : ${SPARK_DIR}/projects/engine
	- For the demos : ${SPARK_DIR}/projects/demos

To build a project (engine or demos):

	First, you have to know how CMake works. If not, there are plenty of tutorials on the net.
	When configuring projects for the first time, CMake will output some errors: it is because
	'SPARK_*' fields are not filled. Fill them, re-configure, generate, and have fun !

Note:

	Libraries are put in the folder ${SPARK_DIR}/lib/<system-name>@<generator>/<build-type>
	where:
		<system-name> is the name of your OS (ex: Windows)
		<generator> is the name of the generator used (ex: Visual Studio 10)
		<build-type> is 'dynamic' or 'static', depending on the project settings. (see SPARK_STATIC_BUILD variable)

	Demos are put in ${SPARK_DIR}/demos/bin