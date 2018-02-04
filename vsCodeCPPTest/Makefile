CC := h5c++#g++#
SRCDIR := src
BINDIR := bin
BUILDDIR := build
EXECUTABLES :=  $(BINDIR)/myApp #$(BINDIR)/test_hdf5 #$(BINDIR)/opt #$(BINDIR)/test_hdf5#$(BINDIR)/svm_ex  #$(BINDIR)/GUItest $(BINDIR)/bayes_net_gui_ex $(BINDIR)/3d_point_cloud_ex $(BINDIR)/surf_ex

SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CPPFLAGS := -g  -Wall -std=c++11 #-DDLIB_NO_GUI_SUPPORT
WXFLAGS := `wx-config --cxxflags` `wx-config --libs`
SHOGUNFLAGS :=  -lshogun
HDF5FLAGS := h5c++ -show
SRCEXT := cpp

LIB := -lm -lpthread -lnsl -lX11 -lhdf5_serial

INC := -I include/ -I lib/ -Ilib/include  -I /usr/lib/x86_64-linux-gnu/hdf5/serial/include
# ------------------------------------------------------------------
all: $(EXECUTABLES)


$(BINDIR)/GUItest: $(BUILDDIR)/GUItest.o $(BUILDDIR)/win.o $(BUILDDIR)/color_box.o $(BUILDDIR)/mdot.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/bayes_net_gui_ex: $(BUILDDIR)/bayes_net_gui_ex.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/3d_point_cloud_ex: $(BUILDDIR)/3d_point_cloud_ex.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/mcex: $(BUILDDIR)/mcex.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/surf_ex: $(BUILDDIR)/surf_ex.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/svm_ex: $(BUILDDIR)/svm_ex.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)

$(BINDIR)/opt: $(BUILDDIR)/opt.o $(BUILDDIR)/source.o
	$(CC)  -O0  $^ -o $@ $(LIB)
	
$(BINDIR)/test_hdf5: $(BUILDDIR)/test_hdf5.o
	$(CC)  -O0  $^ -o $@ $(LIB)


$(BINDIR)/myApp: $(BUILDDIR)/myApp.o  $(BUILDDIR)/myFrame.o $(BUILDDIR)/draw_canvas.o $(BUILDDIR)/BasicPicture.o $(BUILDDIR)/data_point.o $(BUILDDIR)/bin_classifier.o $(BUILDDIR)/measurement_data.o $(BUILDDIR)/helpers.o $(BUILDDIR)/Graph.o $(BUILDDIR)/source.o	
	$(CC)  -Og  $^ -o $@ $(LIB) $(WXFLAGS) #$(SHOGUNFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	$(CC) -c $(CPPFLAGS) $(INC) $^ -o $(BUILDDIR)/$*.o $(WXFLAGS) 

$(BUILDDIR)/source.o: lib/dlib/all/source.cpp
	$(CC) $(CPPFLAGS) lib/dlib/all/source.cpp -c -o $@

clean:
	$(RM) -r $(BUILDDIR) $(BINDIR)