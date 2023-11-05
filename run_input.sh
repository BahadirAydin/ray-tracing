#!/bin/zsh

# Directory containing your input files
xml_dir="/home/bahadir/BahadirAydin/Akademi/CENG477/hw1/ray-tracing/test_scenes/inputs/"

# Path to your raytracer executable
raytracer="./raytracer"

# Function to format the elapsed time in minutes and seconds
format_time() {
    local minutes=$(( $1 / 60 ))
    local seconds=$(( $1 % 60 ))
    echo "$minutes minutes and $seconds seconds"
}

# Loop through all XML files in the directory
for xml_file in "$xml_dir"*.xml; do
    if [ -f "$xml_file" ]; then
        echo "Running raytracer with $xml_file"
        
        # Capture the start time
        start_time=$(date +%s)

        # Run the raytracer and capture its output
        output="$($raytracer "$xml_file")"

        # Capture the end time
        end_time=$(date +%s)

        # Calculate the elapsed time
        elapsed_time=$((end_time - start_time))
        
        # Format and display the elapsed time
        formatted_time=$(format_time $elapsed_time)
        echo "Elapsed time: $formatted_time"
    fi
done

