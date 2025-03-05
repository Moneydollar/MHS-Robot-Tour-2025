import sys
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout, 
                           QGraphicsScene, QGraphicsView, QLabel, QLineEdit, 
                           QPushButton, QGraphicsItem, QGraphicsTextItem, QMenu,
                           QGraphicsProxyWidget, QMessageBox, QComboBox, QDialog, QTextBrowser)
from PyQt5.QtCore import Qt, QRectF, QPointF, QUrl
from PyQt5.QtGui import QPainter, QColor, QPen, QBrush

DARK_THEME = {
    'background': '#1e1e1e',
    'secondary_bg': '#252526',
    'accent': '#2d2d2d',
    'text': '#ffffff',
    'border': '#333333'
}

warning_dialog_style = f"""
    QMessageBox {{
        background-color: {DARK_THEME['background']};
        color: {DARK_THEME['text']};
        border: 1px solid {DARK_THEME['border']};
    }}
    QMessageBox QLabel {{
        color: {DARK_THEME['text']};
    }}
    QMessageBox QPushButton {{
        background-color: {DARK_THEME['accent']};
        color: {DARK_THEME['text']};
        border: 1px solid {DARK_THEME['border']};
        padding: 5px;
        border-radius: 3px;
    }}
    QMessageBox QPushButton:hover {{
        background-color: {DARK_THEME['secondary_bg']};
    }}
"""

help_text_browser_style = f"""
    QTextBrowser {{
        background-color: {DARK_THEME['background']};
        color: {DARK_THEME['text']};
        border: 1px solid {DARK_THEME['border']};
        border-radius: 5px;
        padding: 10px;
    }}
    QTextBrowser::cursor {{
        color: {DARK_THEME['text']};
    }}
    QTextBrowser::text {{
        color: {DARK_THEME['text']};
    }}
"""

class CommandColors:
    MOVE_FORWARD = QColor(65, 184, 131)  # Green
    MOVE_BACKWARD = QColor(234, 84, 85)  # Red
    TURN_LEFT = QColor(250, 152, 58)     # Orange
    TURN_RIGHT = QColor(242, 201, 76)    # Yellow
    DELAY = QColor(83, 123, 196)         # Blue
    
    @staticmethod
    def get_color(command_type):
        color_map = {
            'moveForward': CommandColors.MOVE_FORWARD,
            'moveBackward': CommandColors.MOVE_BACKWARD,
            'turnLeft': CommandColors.TURN_LEFT,
            'turnRight': CommandColors.TURN_RIGHT,
            'delay': CommandColors.DELAY
        }
        return color_map.get(command_type, QColor(180, 180, 180))

class Block(QGraphicsItem):
    BLOCK_HEIGHT = 60
    BLOCK_WIDTH = 280
    SNAP_DISTANCE = 20
    
    def __init__(self, label, command_type, parent=None):
        super().__init__(parent)
        self.label = label
        self.command_type = command_type
        self.color = CommandColors.get_color(command_type)
        self.setFlag(QGraphicsItem.ItemIsMovable)
        self.setFlag(QGraphicsItem.ItemIsSelectable)
        self.setAcceptHoverEvents(True)
        
        self.text_item = QGraphicsTextItem(self)
        self.text_item.setPlainText(self.label)
        self.text_item.setDefaultTextColor(Qt.white)
        self.text_item.setPos(10, 20)
        
        self.input_field = QLineEdit()
        if command_type in ['moveForward', 'moveBackward']:
            self.input_field.setPlaceholderText("Distance (m)")
        elif command_type in ['turnLeft', 'turnRight']:
            self.input_field.setPlaceholderText("Angle (deg)")
        elif command_type == 'delay':
            self.input_field.setPlaceholderText("Time (ms)")
            
        self.input_field.setStyleSheet(f"""
            QLineEdit {{
                background-color: {DARK_THEME['accent']};
                color: {DARK_THEME['text']};
                border: 1px solid {DARK_THEME['border']};
                border-radius: 3px;
                padding: 2px;
            }}
            QLineEdit::placeholder {{
                color: rgba(255, 255, 255, 0.6);
            }}
        """)
        
        if command_type != 'delay':
            self.speed_selector = QComboBox()
            self.speed_selector.addItems(["Max Speed", "Half Speed"])
            self.speed_selector.setStyleSheet(f"""
                QComboBox {{
                    background-color: {DARK_THEME['accent']};
                    color: {DARK_THEME['text']};
                    border: 1px solid {DARK_THEME['border']};
                    border-radius: 3px;
                    padding: 2px;
                }}
                QComboBox QAbstractItemView {{
                    background-color: {DARK_THEME['secondary_bg']};
                    color: {DARK_THEME['text']};
                    selection-background-color: {DARK_THEME['accent']};
                }}
            """)
            self.speed_proxy = QGraphicsProxyWidget(self)
            self.speed_proxy.setWidget(self.speed_selector)
            self.speed_proxy.setPos(120, 30)
        
        self.proxy_widget = QGraphicsProxyWidget(self)
        self.proxy_widget.setWidget(self.input_field)
        self.proxy_widget.setPos(120, 5)

    def boundingRect(self):
        return QRectF(0, 0, self.BLOCK_WIDTH, self.BLOCK_HEIGHT)

    def paint(self, painter, option, widget):
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor(0, 0, 0, 40))
        painter.drawRoundedRect(3, 3, self.BLOCK_WIDTH, self.BLOCK_HEIGHT, 5, 5)
        painter.setPen(QPen(Qt.black, 1))
        painter.setBrush(QBrush(self.color))
        painter.drawRoundedRect(0, 0, self.BLOCK_WIDTH, self.BLOCK_HEIGHT, 5, 5)
        if self.isSelected():
            painter.setPen(QPen(Qt.white, 2, Qt.DashLine))
            painter.setBrush(Qt.NoBrush)
            painter.drawRoundedRect(0, 0, self.BLOCK_WIDTH, self.BLOCK_HEIGHT, 5, 5)

    def mouseMoveEvent(self, event):
        super().mouseMoveEvent(event)
        self.update_position()

    def update_position(self):
        min_distance = float('inf')
        snap_pos = None
        
        for item in self.scene().items():
            if isinstance(item, Block) and item != self:
                bottom_of_other = item.scenePos() + QPointF(0, self.BLOCK_HEIGHT)
                distance = (self.scenePos() - bottom_of_other).manhattanLength()
                
                if distance < min_distance and distance < self.SNAP_DISTANCE:
                    min_distance = distance
                    snap_pos = bottom_of_other
        
        if snap_pos:
            self.setPos(snap_pos)

    def add_block(self, label, command_type):
        pos = self.pos() + QPointF(0, self.BLOCK_HEIGHT + 10)
        self.scene().addBlock(label, command_type, pos)
        
    def delete_block(self):
        self.scene().removeItem(self)

    def contextMenuEvent(self, event):
        menu = QMenu()
        menu.setStyleSheet(f"""
            QMenu {{
                background-color: {DARK_THEME['secondary_bg']};
                color: {DARK_THEME['text']};
                border: 1px solid {DARK_THEME['border']};
            }}
            QMenu::item:selected {{
                background-color: {DARK_THEME['accent']};
            }}
            QMenu::item {{
                padding: 5px 20px;
            }}
        """)
        
        add_forward = menu.addAction("Add Move Forward")
        add_forward.triggered.connect(lambda: self.add_block("Move Forward", "moveForward"))
        
        add_backward = menu.addAction("Add Move Backward")
        add_backward.triggered.connect(lambda: self.add_block("Move Backward", "moveBackward"))
        
        add_left = menu.addAction("Add Turn Left")
        add_left.triggered.connect(lambda: self.add_block("Turn Left", "turnLeft"))
        
        add_right = menu.addAction("Add Turn Right")
        add_right.triggered.connect(lambda: self.add_block("Turn Right", "turnRight"))
        
        add_delay = menu.addAction("Add Delay")
        add_delay.triggered.connect(lambda: self.add_block("Delay", "delay"))
        
        menu.addSeparator()
        delete_action = menu.addAction("Delete Block")
        delete_action.triggered.connect(self.delete_block)
        
        menu.exec_(event.screenPos())

    def get_command(self):
        value = self.input_field.text()
        if not value:
            return None
            
        try:
            value = float(value)
        except ValueError:
            return None
            
        command = {
            'type': self.command_type,
            'value': value,
            'label': self.label
        }
        
        if hasattr(self, 'speed_selector'):
            command['speed'] = "MAX_SPEED" if self.speed_selector.currentText() == "Max Speed" else "HALF_SPEED"
            
        return command

class CustomScene(QGraphicsScene):
    def addBlock(self, label, command_type, pos=None):
        block = Block(label, command_type)
        self.addItem(block)
        if pos:
            block.setPos(pos)
        else:
            block.setPos(20, 20)
        return block

class ScratchApp(QWidget):
    def __init__(self):
        super().__init__()
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle('Arduino Robot Command Generator')
        self.setGeometry(100, 100, 900, 600)
        self.setStyleSheet(f"background-color: {DARK_THEME['background']};")
        
        main_layout = QHBoxLayout()
        left_panel = QVBoxLayout()
        
        title_label = QLabel("Arduino Robot Command Generator")
        title_label.setStyleSheet(f"""
            font-size: 24px;
            font-weight: bold;
            color: {DARK_THEME['text']};
            margin: 10px 0;
        """)
        left_panel.addWidget(title_label)
        
        self.scene = CustomScene()
        self.view = QGraphicsView(self.scene)
        self.view.setRenderHint(QPainter.Antialiasing)
        self.view.setViewportUpdateMode(QGraphicsView.FullViewportUpdate)
        self.view.setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.view.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        self.view.setStyleSheet(f"""
            QGraphicsView {{
                background-color: {DARK_THEME['secondary_bg']};
                border: 1px solid {DARK_THEME['border']};
                border-radius: 5px;
            }}
        """)
        left_panel.addWidget(self.view)
        
        right_panel = QVBoxLayout()
        blocks_label = QLabel("Command Blocks")
        blocks_label.setStyleSheet(f"""
            font-size: 18px;
            font-weight: bold;
            color: {DARK_THEME['text']};
            margin: 10px 0;
        """)
        right_panel.addWidget(blocks_label)
        
        self.create_command_buttons(right_panel)
        
        self.output_label = QLabel("Generated Arduino Code:")
        self.output_label.setStyleSheet(f"""
            font-size: 16px;
            color: {DARK_THEME['text']};
            margin-top: 20px;
        """)
        right_panel.addWidget(self.output_label)
        
        command_output_layout = QHBoxLayout()
        self.command_output = QLabel()
        self.command_output.setStyleSheet(f"""
            QLabel {{
                background-color: {DARK_THEME['accent']};
                color: {DARK_THEME['text']};
                border: 1px solid {DARK_THEME['border']};
                border-radius: 5px;
                padding: 10px;
                min-height: 150px;
                font-family: monospace;
            }}
        """)
        self.command_output.setWordWrap(True)
        
        self.copy_button = QPushButton("Copy Code")
        self.copy_button.setStyleSheet("""
            QPushButton {
                background-color: #2196F3;
                color: white;
                font-size: 14px;
                padding: 8px;
                border-radius: 4px;
                margin: 5px;
            }
            QPushButton:hover {
                background-color: #1976D2;
            }
        """)
        self.copy_button.clicked.connect(self.copy_generated_code)
        self.copy_button.setEnabled(False)  # Initially disabled
        
        command_output_layout.addWidget(self.command_output)
        command_output_layout.addWidget(self.copy_button)
        
        right_panel.addLayout(command_output_layout)

        generate_button = QPushButton("Generate Arduino Code")
        generate_button.setStyleSheet("""
            QPushButton {
                background-color: #4CAF50;
                color: white;
                font-size: 16px;
                padding: 10px;
                border-radius: 5px;
                border: none;
            }
            QPushButton:hover {
                background-color: #45a049;
            }
        """)
        generate_button.clicked.connect(self.generate_commands)
        right_panel.addWidget(generate_button)

        help_button = QPushButton("Help")
        help_button.setStyleSheet("""
            QPushButton {
                background-color: #2196F3;
                color: white;
                font-size: 14px;
                padding: 8px;
                border-radius: 4px;
                margin: 5px;
            }
            QPushButton:hover {
                background-color: #1976D2;
            }
        """)
        help_button.clicked.connect(self.show_help)
        right_panel.addWidget(help_button)
        
        main_layout.addLayout(left_panel, 2)
        main_layout.addLayout(right_panel, 1)
        
        self.setLayout(main_layout)

    def show_help(self):
        help_dialog = QDialog(self)
        help_dialog.setWindowTitle("Help")
        help_dialog.setGeometry(100, 100, 800, 600)
        
        text_browser = QTextBrowser(help_dialog)
        text_browser.setOpenExternalLinks(True)
        text_browser.setStyleSheet(help_text_browser_style)

        help_file_path = QUrl.fromLocalFile("./help.md")
        text_browser.setSource(help_file_path)

        layout = QVBoxLayout(help_dialog)
        layout.addWidget(text_browser)
        
        help_dialog.setLayout(layout)
        help_dialog.exec_()

    def create_command_buttons(self, layout):
        commands = [
            ("Move Forward", "moveForward"),
            ("Move Backward", "moveBackward"),
            ("Turn Left", "turnLeft"),
            ("Turn Right", "turnRight"),
            ("Delay", "delay")
        ]
        
        for label, cmd_type in commands:
            btn = QPushButton(f"Add {label}")
            color = CommandColors.get_color(cmd_type)
            btn.setStyleSheet(f"""
                QPushButton {{
                    background-color: {color.name()};
                    color: white;
                    padding: 8px;
                    border-radius: 4px;
                    margin: 5px;
                }}
                QPushButton:hover {{
                    background-color: {color.darker(120).name()};
                }}
            """)
            btn.clicked.connect(lambda checked, l=label, t=cmd_type: 
                              self.scene.addBlock(l, t))
            layout.addWidget(btn)

    def generate_commands(self):
        try:
            commands = []
            for item in self.scene.items():
                if isinstance(item, Block):
                    command = item.get_command()
                    if command:
                        commands.append(command)
            
            if not commands:
                QMessageBox.warning(self, "Warning", 
                                  "No commands to generate. Please add some blocks and enter values.")
                return
            
            code = "void loop() {\n"
            for cmd in commands:
                if cmd['type'] in ['moveForward', 'moveBackward']:
                    code += f"  {cmd['type']}({cmd['speed']}, {cmd['value']});\n"
                elif cmd['type'] in ['turnLeft', 'turnRight']:
                    code += f"  {cmd['type']}({cmd['speed']}, {cmd['value']});\n"
                elif cmd['type'] == 'delay':
                    code += f"  delay({int(cmd['value'])});\n"
            
            code += "\n  while(1); // Halt the program\n}"
            
            self.command_output.setText(code)
            self.copy_button.setEnabled(True)  # Enable copy button when code is generated
            
        except ValueError as e:
            QMessageBox.warning(self, "Error", 
                              "Please enter valid numerical values for all commands.")

    def copy_generated_code(self):
        clipboard = QApplication.clipboard()
        clipboard.setText(self.command_output.text())
        
        # Optional: Show a temporary tooltip or message
        QMessageBox.information(self, "Copied", "Arduino code copied to clipboard!")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = ScratchApp()
    window.show()
    sys.exit(app.exec_())