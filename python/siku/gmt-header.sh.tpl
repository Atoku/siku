#!/bin/bash
#
# x - latitude
# y - longitude

FILEROOT="{fileroot}"
DATA="{input}"

# Defaults upgrade
PS_MEDIA="letter"
MAP_TICK_PEN_PRIMARY="thinner,black"
MAP_TICK_PEN_SECONDARY="thinner,black"
MAP_GRID_PEN_PRIMARY="default,139/140/200"
MAP_GRID_PEN_SECONDARY="thinner,gray"
MAP_VECTOR_SHAPE="2"

gmt set \
    --PS_MEDIA=$PS_MEDIA \
    --MAP_TICK_PEN_PRIMARY=$MAP_TICK_PEN_PRIMARY \
    --MAP_TICK_PEN_SECONDARY=$MAP_TICK_PEN_SECONDARY \
    --MAP_GRID_PEN_PRIMARY=$MAP_GRID_PEN_PRIMARY \
    --MAP_GRID_PEN_SECONDARY=$MAP_GRID_PEN_SECONDARY \
    --MAP_VECTOR_SHAPE=$MAP_VECTOR_SHAPE

# Main parameters
PROJECTION="{projection}"
REGION="{region}"
BGRID_LINES="g45/g45"            # lat/lon lines
RESOLUTION="i"                  # f,h,i.l,c
FILL_LAND_COLOR="187/142/46"
FILL_SEA_COLOR="109/202/255"
NATIONAL_BORDERS="1"
# PROJECTION="B-154/50/55/65/16c"
# REGION="172/243/62/80"
# BGRID_LINES="p10g5/5g5"
# RESOLUTION="i"                  # f,h,i,l,c
# SCALE_BAR="215/64/64/1000"

#FILL_LAND_COLOR="255/206/144"
FILL_LAND_COLOR="255/226/164"
#FILL_LAND_COLOR="64/64/64"

FILL_SEA_COLOR="169/207/254"
#FILL_SEA_COLOR="26/105/122"

SEA_ICE_COLOR="255/255/255"

SEA_NAMES_COLOR="63/153/255"
NATIONAL_BORDERS="1"

# First command, no -O option and single > redirection

gmt pscoast \
    -J$PROJECTION \
    -R$REGION \
    -D$RESOLUTION \
    -G$FILL_LAND_COLOR \
    -S$FILL_SEA_COLOR \
    -B$BGRID_LINES \
    -P -K \
    --PS_MEDIA=$PS_MEDIA \
    > $FILEROOT.ps

# ------------------
# For Emacs Users
# Local Variables:
# mode: sh
# comment-start: "#"
# End:
