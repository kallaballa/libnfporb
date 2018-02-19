truncate --size 0 testgen.log

while [ 0 ]; do
  file1="`ls data/generated/* | shuf | head -n1`"
  file2="`ls data/generated/* | shuf | head -n1`" 

  echo -n "#### Running case $file1 $file2: "
  examples/nfp $file1 $file2 &>> testgen.log
  if [ $? -eq 0 ]; then
    echo Success
  else
    echo Fail
    exit 1;
  fi
done
