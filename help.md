# Arduino Robot Command Generator - User Guide

## Getting Started

The Arduino Robot Command Generator is a visual programming tool that helps you create movement sequences for your Arduino-based robot.

## Installation

### Requirements

- Python 3.7 or higher
- PyQt5
- Arduino IDE

### Installing from Source

1. Install Python from [python.org](https://python.org)
2. Install PyQt5: `pip install PyQt5`
3. Run the script: `python robot_command_generator.py`

### Creating an Executable

1. Install PyInstaller: `pip install pyinstaller`
2. Open a terminal in the script directory
3. Run: `pyinstaller --onefile --windowed robot_command_generator.py`
4. Find the executable in the `dist` folder

## Using the Application

### Command Blocks

**Available Commands:**

- **Move Forward** - Distance in meters
- **Move Backward** - Distance in meters
- **Turn Left** - Angle in degrees
- **Turn Right** - Angle in degrees
- **Delay** - Time in milliseconds

### Creating a Sequence

1. Click the command buttons on the right to add blocks
2. Drag blocks to reposition them
3. Enter values in the input fields
4. Select speed (Max/Half) for movement commands
5. Right-click blocks to add more or delete

### Generating Code

1. Add all desired command blocks
2. Fill in all required values
3. Click "Generate Arduino Code"
4. Copy the generated code to your Arduino IDE

## Tips and Tricks

- Blocks will snap together when dragged close to each other
- Use delays between movements for smoother operation
- Right-click any block to add new blocks below it
- Invalid values will be highlighted when generating code

### Troubleshooting

- If blocks won't snap, ensure they're close enough (within 20 pixels)
- If code generation fails, check all input fields have valid numbers
- For movement issues, try adding short delays between commands
