#!/bin/sh
# generate tag file for lookupfile plugin
echo "test source"
echo -e "!_TAG_FILE_SORTED\t2\t/2=foldcase/" > gtags_files
#find . -not -regex '.*\.\(png\|gif\|pbi\|o\|out\|cout\|html\|pack\|idx\)' \
#	   -not -path "./.git/*" \
#	   -type f -printf "%f\t%p\t1\n" | sort -f >> filenametags

# -path "./Documentation*" -prune -o \
# -name "*.[chxsS]" -print > cscope.files
#	-path "./BL0/EWARM/*" -prune -o \
#	-path "./BL1/EWARM/*" -prune -o \
#	-path "./BL1/*" -prune -o \
find . \
	-path "./.git/*" -prune -o \
	-path "./bootloader/*" -prune -o \
	-path "./G*" -prune -o \
	-path "./gtags*" -prune -o \
	-path "./system/EWARM/*" -prune -o \
	-path "./settings/*" -prune -o \
	-print > gtags_files
