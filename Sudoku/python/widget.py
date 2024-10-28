import sys
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QWidget,
    QPushButton,
    QAction,
    qApp,
    QDesktopWidget,
)
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QCoreApplication


class MyApp(QWidget):

    def __init__(self):
        super().__init__()

        btn = QPushButton("Quit", self)
        btn.move(50, 50)
        btn.resize(btn.sizeHint())
        btn.clicked.connect(QCoreApplication.instance().quit)

        exitAction = QAction(QIcon("sudoku.png"), "Exit", self)
        exitAction.setShortcut("Ctrl+Q")
        exitAction.setStatusTip("Exit application")
        exitAction.triggered.connect(qApp.quit)

        # menubar = self.menuBar()
        # # menubar.setNativeMenuBar(False)
        # filemenu = menubar.addMenu('&File')
        # filemenu.addAction(exitAction)

        self.setWindowTitle("Sudoku")
        self.setWindowIcon(QIcon("sudoku.png"))
        self.setGeometry(300, 300, 300, 200)
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())
        self.show()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())
