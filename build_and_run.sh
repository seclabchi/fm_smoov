mkdir -p outputs

cmake .
make
mv ./cmake-build-Debug/output/fmsmoovd ./outputs/

cd fmsmoov_ui
cmake .
make
cd ..
mv ./cmake-build-Debug/output/fmsmoov_ui ./outputs/

