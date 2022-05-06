
SPARK Particle Engine
=====================

<img src="demos/bin/img/collision.jpg?raw=true" width="160" height="120" alt="Collision demo">
<img src="demos/bin/img/explosion.jpg?raw=true" width="160" height="120" alt="Explosion demo">
<img src="demos/bin/img/fire.jpg?raw=true" width="160" height="120" alt="Fire demo">

<img src="demos/bin/img/flakes.jpg?raw=true" width="160" height="120" alt="Flakes demo">
<img src="demos/bin/img/galaxy.jpg?raw=true" width="160" height="120" alt="Galaxy demo">
<img src="demos/bin/img/gravitation.jpg?raw=true" width="160" height="120" alt="Gravitation demo">

Build guide
-----------

#### Building the SPARK library and the demos

	git clone https://github.com/Synxis/SPARK
	cd SPARK
	mkdir build
	cd build
	cmake -DSPARK_BUILD_DEMOS:BOOL="1" ../projects
	cmake --build . --config Release

#### Running a demo

	cd SPARK/demos/bin
	./Test_Simple (or just 'Test_Simple.exe' on Windows)

#### Building the SPARK library with Irrlicht support

	cmake -DSPARK_BUILD_DEMOS:BOOL="1" \
		  -DSPARK_BUILD_IRRLICHT_MODULE:BOOL="1" \
		  -DSPARK_IRRLICHT_LIB:FILEPATH="/path/to/the/irrlicht/lib/filename" \
		  -DSPARK_IRRLICHT_INCLUDE_DIR:PATH="/path/to/the/folder/with/the/irrlicht/headers/"

	cmake --build . --config Release

#### Extra notes

By default the compiled libraries and demos will be placed in:

        SPARK
        ├── lib/
        │   └── <system-name>@<generator>/
        |       └── static/
        |       └── dynamic/
        └── demos/
            └── bin/

- *\<system-name>* is the name of your OS (ex: Windows)
- *\<generator>* is the name of the generator used (ex: Visual Studio 10)

Available cmake variables:

- `SPARK_USE_HARCODED_RUNTIME_OUTPUT_DIRS` if set to `FALSE` it will make cmake use the *build* directory to output all the results of the compilation instead of using the hardcoded directory layout above.

- `SPARK_STATIC_BUILD` controls whether SPARK is built as a static or dynamic library.

- `SPARK_IRRLICHT_USE_STATIC_LIBS` controls whether SPARK will be linked with a static or dynamic Irrlicht library.

- `SPARK_IRRLICHT_DEBUG` is only available when using cmake with multi configuration generators. This variable sets the path to the file of the debug Irrlicht library. If using a visual studio generator without setting the `SPARK_IRRLICHT_DEBUG` variable the `Debug` configuration won't be available.
Here's an example:

		cmake -DSPARK_BUILD_DEMOS:BOOL="1" \
			  -DSPARK_BUILD_IRRLICHT_MODULE:BOOL="1" \
			  -DSPARK_IRRLICHT_LIB:FILEPATH="/path/to/the/irrlicht/release/lib/filename" \
			  -DSPARK_IRRLICHT_LIB_DEBUG:FILEPATH="/path/to/the/irrlicht/debug/lib/filename" \
			  -DSPARK_IRRLICHT_INCLUDE_DIR:PATH="/path/to/the/folder/with/the/irrlicht/headers/"

		cmake --build . --config Release
		cmake --build . --config Debug
