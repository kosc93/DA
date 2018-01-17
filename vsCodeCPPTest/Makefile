CC := g++
SRCDIR := src
BINDIR := bin
BUILDDIR := build
EXECUTABLES :=  $(BINDIR)/myApp  $(BINDIR)/svm_ex #$(BINDIR)/GUItest $(BINDIR)/bayes_net_gui_ex $(BINDIR)/3d_point_cloud_ex $(BINDIR)/surf_ex

SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CPPFLAGS := -g -Wall -std=c++11 #-DDLIB_NO_GUI_SUPPORT
WXFLAGS := `wx-config --cxxflags` `wx-config --libs`
SHOGUNFLAGS :=  -lshogun
SRCEXT := cpp

LIB := -lm -lpthread -lnsl -lX11 

INC := -I include/ -I lib/ 
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

$(BINDIR)/myApp: $(BUILDDIR)/myApp.o  $(BUILDDIR)/myFrame.o $(BUILDDIR)/draw_canvas.o $(BUILDDIR)/BasicPicture.o $(BUILDDIR)/data_point.o $(BUILDDIR)/bin_classifier.o	
	$(CC)  -O0  $^ -o $@ $(LIB) $(WXFLAGS) #$(SHOGUNFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	$(CC) -c $(CPPFLAGS) $(INC) $^ -o $(BUILDDIR)/$*.o $(WXFLAGS) #$(SHOGUNFLAGS)

$(BUILDDIR)/source.o: lib/dlib/all/source.cpp
	$(CC) $(CPPFLAGS) lib/dlib/all/source.cpp -c -o $@

clean:
	$(RM) -r $(BUILDDIR) $(BINDIR)