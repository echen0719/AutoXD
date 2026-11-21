import sys
import time
from PyQt6.QtWidgets import QApplication, QWidget, QPushButton, QLabel, QVBoxLayout

class CPSTest(QWidget):
    def __init__(self):
        super().__init__()

        self.clicks = 0
        self.start = 0
        self.duration = 5

        self.label = QLabel("Click to start")
        self.button = QPushButton("Click here")
        self.reset = QPushButton("Reset clicks")

        layout = QVBoxLayout()
        layout.addWidget(self.label)
        layout.addWidget(self.button)
        layout.addWidget(self.reset)
        self.setLayout(layout)
        self.setFixedWidth(300)

        self.button.clicked.connect(self.click)
        self.reset.clicked.connect(self.restart)

    def click(self):
        if self.start == 0:
            self.start = time.time()

        if time.time() - self.start < self.duration:
            self.clicks += 1
            self.label.setText("Clicks: {}".format(self.clicks))
        else:
            self.label.setText("CPS: {:.2f}".format(self.clicks / self.duration))
            self.button.setEnabled(False)

    def restart(self):
        self.clicks = 0
        self.start = 0
        self.label.setText("Click to start")
        self.button.setEnabled(True)

app = QApplication(sys.argv)
window = CPSTest()
window.show()
sys.exit(app.exec())