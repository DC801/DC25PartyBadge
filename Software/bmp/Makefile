#
# Makefile for the BMP to RGB565 convertor utility
#

IMAGESRCDIR = images-in
IMAGEOUTDIR = images-out
APP = bmp-convert
CFLAGS += -Wall -g -rdynamic -funwind-tables
LDFLAGS +=
BUILDDIR = build

SRCS = bmp-convert.c

OBJS = $(SRCS:%.c=$(BUILDDIR)/%.o)
DEPS := $(SRCS:%.c=$(BUILDDIR)/%.d)
DF = $(BUILDDIR)/$(*F)

IMAGES := $(shell find $(IMAGESRCDIR) -name '*.bmp')
CONVIMAGES = $(IMAGES:$(IMAGESRCDIR)/%.bmp=$(IMAGEOUTDIR)/%.h)

.PHONY: all clean image-convert $(IMAGES)

all: $(APP)

$(APP): $(OBJS)
	@echo " linking : " $(OBJS) " => " $@
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@
	@echo " convert app built"
	
$(BUILDDIR)/%.o: %.c
	-@mkdir -p $(BUILDDIR)
	@echo " gcc : " $< " => " $@ 
	@$(CC) $(CFLAGS) -MM -MP -MT $(DF).o -MT $(DF).d $(CFLAGS) $(EXTRA_CFLAGS) $< > $(DF).d
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@
	
$(IMAGEOUTDIR)/%.h: $(IMAGESRCDIR)/%.bmp
	@mkdir -p $(IMAGEOUTDIR)
	@echo " convert : " $< " => " $@
	@./$(APP) $< $@

image-convert: $(APP) $(CONVIMAGES)
	@echo " all images compiled to headers"

clean:
	@find . -name "*.o" -delete
	-@rm -f $(APP) 
	-@rm -rf $(BUILDDIR)
	-@rm -f $(IMAGEOUTDIR)/*.h
	
-include $(DEPS)

	