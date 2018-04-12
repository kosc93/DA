CC := g++ -O0 -g #h5c++##
SRCDIR := src
BINDIR := bin
BUILDDIR := build
EXECUTABLES :=   $(BINDIR)/vo_klass $(BINDIR)/h5_helper  $(BINDIR)/dlib_test #$(BINDIR)/myApp $(BINDIR)/test_hdf5 #$(BINDIR)/opt #$(BINDIR)/test_hdf5#$(BINDIR)/svm_ex  #$(BINDIR)/GUItest $(BINDIR)/bayes_net_gui_ex $(BINDIR)/3d_point_cloud_ex $(BINDIR)/surf_ex

SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CPPFLAGS := -Wall -std=c++11 -DDLIB_NO_GUI_SUPPORT #-DDLIB_USE_LAPACK#-DDLIB_USE_BLAS #
WXFLAGS := `wx-config --cxxflags` `wx-config --libs`
SHOGUNFLAGS :=  -lshogun -L /usr/lib/libshogun.*
HDF5FLAGS :=  -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE #-D_FORTIFY_SOURCE=2
SRCEXT := cpp


#-lblas -llapack
LIB :=  -lsz -lz -ldl -lm -lpthread -lnsl -lX11 -lhdf5_serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_cpp.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5_hl.a /usr/lib/x86_64-linux-gnu/hdf5/serial/libhdf5.a

INC := -I include/ -I lib/ -I lib/include  -I /usr/lib/x86_64-linux-gnu/hdf5/serial/include -I/usr/include/hdf5/serial
# ------------------------------------------------------------------
all: $(EXECUTABLES)


$(BINDIR)/dlib_test: $(BUILDDIR)/dlib_test.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/bayes_net_gui_ex: $(BUILDDIR)/bayes_net_gui_ex.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/3d_point_cloud_ex: $(BUILDDIR)/3d_point_cloud_ex.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/mcex: $(BUILDDIR)/mcex.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/cmd_parser: $(BUILDDIR)/cmd_parser.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/svm_ex: $(BUILDDIR)/svm_ex.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)

$(BINDIR)/shogunTest: $(BUILDDIR)/shogunTest.o $(BUILDDIR)/source.o
	$(CC)   $^ -o $@ $(LIB)
	
$(BINDIR)/h5_helper: $(BUILDDIR)/h5_helper.o $(BUILDDIR)/measurement_data.o  $(BUILDDIR)/classifier.o  $(BUILDDIR)/source.o 
	$(CC)   $^ -o $@ $(LIB) $(HDF5FLAGS) 
	
$(BINDIR)/vo_klass: $(BUILDDIR)/vo_klass.o $(BUILDDIR)/measurement_data.o  $(BUILDDIR)/preprocessing.o $(BUILDDIR)/classifier.o $(BUILDDIR)/data_gateway.o $(BUILDDIR)/source.o  
	$(CC)  $^ -o $@ $(LIB)

$(BINDIR)/myApp: $(BUILDDIR)/myApp.o  $(BUILDDIR)/myFrame.o $(BUILDDIR)/draw_canvas.o $(BUILDDIR)/BasicPicture.o $(BUILDDIR)/data_point.o $(BUILDDIR)/classifier.o $(BUILDDIR)/measurement_data.o $(BUILDDIR)/Graph.o $(BUILDDIR)/custom_widgets.o $(BUILDDIR)/source.o	
	$(CC)  $^ -o $@ $(LIB) $(WXFLAGS) #$(SHOGUNFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	$(CC)  -c $(CPPFLAGS) $(INC) $^ -o $(BUILDDIR)/$*.o  #$(WXFLAGS) 

$(BUILDDIR)/source.o: lib/dlib/all/source.cpp
	$(CC) $(CPPFLAGS) lib/dlib/all/source.cpp -c -o $@

clean:
	$(RM) -r $(BUILDDIR) $(BINDIR)
	