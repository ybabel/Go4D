.SUFFIXES: .o .cpp

# Object files
OBJECT = fileio.o \
         lut.o \
         mesh.o \
         mesh_hier.o \
         mesh_utils.o \
         object.o \
         pair.o \
         scene.o

SOURCES = $(OBJECT:.o=.cpp)

.cpp.o:
	$(CC) -c $(CFLAGS) $*.cpp

all: clean $(OBJECT)

depend:
	makedepend -Y -- $(CFLAGS) -- $(SOURCES)

clean:
	rm -f *.o

# DO NOT DELETE

fileio.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
fileio.o: ../include/SWIFT_array.h ../include/SWIFT_fileio.h
lut.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
lut.o: ../include/SWIFT_linalg.h ../include/SWIFT_mesh.h
lut.o: ../include/SWIFT_array.h ../include/SWIFT_lut.h
mesh.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
mesh.o: ../include/SWIFT_linalg.h ../include/SWIFT_array.h
mesh.o: ../include/SWIFT_mesh.h ../include/SWIFT_lut.h
mesh.o: ../include/SWIFT_fileio.h ../include/SWIFT_mesh_utils.h
mesh_utils.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
mesh_utils.o: ../include/SWIFT_linalg.h ../include/SWIFT_array.h
mesh_utils.o: ../include/SWIFT_mesh.h ../include/SWIFT_lut.h
mesh_utils.o: ../include/SWIFT_mesh_utils.h
object.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
object.o: ../include/SWIFT_linalg.h ../include/SWIFT_mesh.h
object.o: ../include/SWIFT_array.h ../include/SWIFT_lut.h
object.o: ../include/SWIFT_object.h ../include/SWIFT_pair.h
object.o: ../include/SWIFT_boxnode.h
pair.o: ../include/SWIFT_config.h ../include/SWIFT_common.h
pair.o: ../include/SWIFT_linalg.h ../include/SWIFT_array.h
pair.o: ../include/SWIFT_mesh.h ../include/SWIFT_lut.h
pair.o: ../include/SWIFT_object.h ../include/SWIFT_pair.h
pair.o: ../include/SWIFT_boxnode.h
scene.o: ../include/SWIFT.h ../include/SWIFT_config.h
scene.o: ../include/SWIFT_common.h ../include/SWIFT_array.h
scene.o: ../include/SWIFT_linalg.h ../include/SWIFT_mesh.h
scene.o: ../include/SWIFT_lut.h ../include/SWIFT_mesh_utils.h
scene.o: ../include/SWIFT_boxnode.h ../include/SWIFT_object.h
scene.o: ../include/SWIFT_pair.h ../include/SWIFT_fileio.h
