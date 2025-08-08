#!/bin/bash
# Script to fix ESP-IDF "dirty" status

echo "This script will help fix your ESP-IDF installation"
echo "Current ESP-IDF path: /Users/chris/esp/v5.5/esp-idf"
echo ""

# Change to ESP-IDF directory
cd /Users/chris/esp/v5.5/esp-idf

echo "Checking ESP-IDF status..."
git status

echo ""
echo "Options:"
echo "1. Stash changes (temporary save)"
echo "2. Discard all changes (reset to clean state)"
echo "3. View detailed changes"
echo "4. Exit without changes"
echo ""
read -p "Choose option (1-4): " choice

case $choice in
    1)
        echo "Stashing changes..."
        git stash
        echo "Changes stashed. You can retrieve them later with 'git stash pop'"
        ;;
    2)
        echo "WARNING: This will discard ALL local changes in ESP-IDF!"
        read -p "Are you sure? (y/N): " confirm
        if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
            echo "Resetting ESP-IDF..."
            git reset --hard
            git clean -f -d
            echo "ESP-IDF reset to clean state"
        else
            echo "Cancelled"
            exit 1
        fi
        ;;
    3)
        echo "Showing detailed changes..."
        git diff
        exit 0
        ;;
    4)
        echo "Exiting without changes"
        exit 0
        ;;
    *)
        echo "Invalid option"
        exit 1
        ;;
esac

echo ""
echo "Updating submodules..."
git submodule update --init --recursive

echo ""
echo "Verifying ESP-IDF version..."
cd /Users/chris/Documents/Code/Little_Dawn
source /Users/chris/esp/v5.5/esp-idf/export.sh
idf.py --version

echo ""
echo "Done! Your ESP-IDF should now be clean."