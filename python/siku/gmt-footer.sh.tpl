# finalize (-T option ignores all inputs)
psxy -R -J -O -T  >> $FILEROOT.ps

# eps production
rm -f  {output}
ps2eps $FILEROOT.ps
rm -f  $FILEROOT.ps
mv $FILEROOT.eps {output}

# ------------------
# For Emacs Users
# Local Variables:
# mode: sh
# comment-start: "#"
# End:
