#!/bin/bash

# Make sure premake5 is available
if ! command -v premake5 &> /dev/null; then
    echo "Error: premake5 not found. Please install premake5 or make sure it's in your PATH."
    echo "You can install it via Homebrew: brew install premake5"
    exit 1
fi

# Generate Makefiles
premake5 gmake2

echo "Makefiles generated successfully!"
echo "You can now build the project with 'make' or 'make config=release'"