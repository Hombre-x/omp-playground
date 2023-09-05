# Bash script to run the pi_approximation_omp program with different number of threads

set numberOfThreads 1 2 4 8 12 16
set currentPath (pwd)

# Compile the programs
echo "Compiling programs..."
gcc -o pi_approximation_omp_fs $currentPath/app/pi_approximation_omp_fs.c -fopenmp -lm -std=c99
gcc -o pi_approximation_omp_ts $currentPath/app/pi_approximation_omp_ts.c -fopenmp -lm -std=c99 
echo \n

set leibnizFs $currentPath/pi_approximation_omp_fs
set leibnizTs $currentPath/pi_approximation_omp_ts

# Run the benchmarks
echo "Stating benchmarks..."
echo "Benchmarks with false sharing:" \n
for threads in $numberOfThreads

  set start (date +%s%N)

  $leibnizFs $threads

  set end (date +%s%N)

  set duration (math "($end - $start) / 1000000000")

  set -a falseSharingTimes "$duration"

  echo "Threads: $threads, Time: $duration" \n
end

echo "Benchmarks with true sharing:" \n
for threads in $numberOfThreads

  set start (date +%s%N)

  $leibnizTs $threads

  set end (date +%s%N)

  set duration (math "($end - $start) / 1000000000")
  set -a trueSharingTimes "$duration"

  echo "Threads: $threads, Time: $duration" \n
end

echo "Benchmarks finished, showing results: " \n

echo "| Thread | False Sharing | True  Sharing |" \n

for i in (seq 1 6)
  printf "| %6d |   %0.7f   |   %0.7f   |" $numberOfThreads[$i] $falseSharingTimes[$i] $trueSharingTimes[$i] 
  echo \n
end

# Delete the executables
rm $leibnizFs
rm $leibnizTs