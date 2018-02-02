
echo "Making output directory ./outputs..."
mkdir -p outputs

echo "Building fmsmoov daemon..."
echo "Running cmake..."
cmake .
make
echo "Moving fmsmoovd build output into outputs directory..."
mv ./cmake-build-Debug/output/fmsmoovd ./outputs/

echo "Building fmsmoov UI..."
cd fmsmoov_ui
cmake .
make
cd ..

echo "Moving fmsmoov UI output into outputs directory..."
mv ./cmake-build-Debug/output/fmsmoov_ui ./outputs/

echo "Done!  Please run the outputs from the outputs directory.  Start fmsmoovd first, then run the UI."
