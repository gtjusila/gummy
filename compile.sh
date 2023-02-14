if [ -d "build" ]; then
  # Take action if $DIR exists. #
  rm -r build
fi
cmake -S src -B build
cd build
make
cd ..
#./build/gummy ./problem_instances/qplib/instances/QPLIB_${1}.qs ./problem_instances/qplib/solution/QPLIB_${1}_answer.qs ${2}