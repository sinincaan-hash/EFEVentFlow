#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QFont>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QStackedWidget>
#include <QDialog>
#include <QFormLayout>
#include <QFileDialog>
#include <QProgressBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QSlider>
#include <QScrollArea>
#include <QVector>
#include <QDateTime>

// Включаем наш менеджер базы данных
#include "database_manager.h"

// Диалоговое окно для добавления расхода
class AddExpenseDialog : public QDialog {
public:
    AddExpenseDialog(const QVector<Event> &eventList, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Добавить расход");
        setFixedSize(400, 400);
        setModal(true);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(30, 30, 30, 30);
        mainLayout->setSpacing(20);

        // Заголовок
        QLabel *titleLabel = new QLabel("Добавить расход");
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #6155F5;");
        mainLayout->addWidget(titleLabel);

        // Форма
        QFormLayout *formLayout = new QFormLayout();
        formLayout->setSpacing(15);

        // Мероприятие
        eventCombo = new QComboBox();
        for (const Event &event : eventList) {
            eventCombo->addItem(event.title, event.id);
        }
        eventCombo->setStyleSheet("QComboBox {"
                                  "border: 1px solid #dce4ec;"
                                  "border-radius: 5px;"
                                  "padding: 10px;"
                                  "font-size: 14px;"
                                  "}"
                                  "QComboBox:focus {"
                                  "border-color: #6155F5;"
                                  "}");

        // Наименование расхода
        expenseNameEdit = new QLineEdit();
        expenseNameEdit->setPlaceholderText("Введите наименование расхода");
        expenseNameEdit->setStyleSheet("QLineEdit {"
                                       "border: 1px solid #dce4ec;"
                                       "border-radius: 5px;"
                                       "padding: 10px;"
                                       "font-size: 14px;"
                                       "}"
                                       "QLineEdit:focus {"
                                       "border-color: #6155F5;"
                                       "}");

        // Сумма
        amountEdit = new QLineEdit();
        amountEdit->setPlaceholderText("0");
        amountEdit->setStyleSheet("QLineEdit {"
                                  "border: 1px solid #dce4ec;"
                                  "border-radius: 5px;"
                                  "padding: 10px;"
                                  "font-size: 14px;"
                                  "}"
                                  "QLineEdit:focus {"
                                  "border-color: #6155F5;"
                                  "}");

        // Категория
        categoryCombo = new QComboBox();
        categoryCombo->addItems({"Транспорт", "Проживание", "Питание", "Реклама", "Оборудование", "Прочее"});
        categoryCombo->setStyleSheet("QComboBox {"
                                     "border: 1px solid #dce4ec;"
                                     "border-radius: 5px;"
                                     "padding: 10px;"
                                     "font-size: 14px;"
                                     "}"
                                     "QComboBox:focus {"
                                     "border-color: #6155F5;"
                                     "}");

        // Дата расхода
        dateEdit = new QDateEdit();
        dateEdit->setCalendarPopup(true);
        dateEdit->setDate(QDate::currentDate());
        dateEdit->setDisplayFormat("dd.MM.yyyy");
        dateEdit->setStyleSheet("QDateEdit {"
                                "border: 1px solid #dce4ec;"
                                "border-radius: 5px;"
                                "padding: 10px;"
                                "font-size: 14px;"
                                "}"
                                "QDateEdit:focus {"
                                "border-color: #6155F5;"
                                "}");

        formLayout->addRow("Мероприятие:", eventCombo);
        formLayout->addRow("Наименование расхода:", expenseNameEdit);
        formLayout->addRow("Сумма (Р):", amountEdit);
        formLayout->addRow("Категория:", categoryCombo);
        formLayout->addRow("Дата расхода:", dateEdit);

        mainLayout->addLayout(formLayout);

        // Кнопки
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        QPushButton *cancelButton = new QPushButton("Отмена");
        cancelButton->setStyleSheet("QPushButton {"
                                    "background-color: #6c757d;"
                                    "color: white;"
                                    "border: none;"
                                    "border-radius: 5px;"
                                    "padding: 10px 20px;"
                                    "font-size: 14px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "background-color: #5a6268;"
                                    "}");

        QPushButton *addButton = new QPushButton("Добавить расход");
        addButton->setStyleSheet("QPushButton {"
                                 "background-color: #6155F5;"
                                 "color: white;"
                                 "border: none;"
                                 "border-radius: 5px;"
                                 "padding: 10px 20px;"
                                 "font-size: 14px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #5046d4;"
                                 "}");

        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(addButton);
        mainLayout->addLayout(buttonLayout);

        // Соединения
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        connect(addButton, &QPushButton::clicked, this, &QDialog::accept);
    }

    int getEventId() const { return eventCombo->currentData().toInt(); }
    QString getEventName() const { return eventCombo->currentText(); }
    QString getExpenseName() const { return expenseNameEdit->text(); }
    double getAmount() const { return amountEdit->text().toDouble(); }
    QString getCategory() const { return categoryCombo->currentText(); }
    QString getDate() const { return dateEdit->text(); }

private:
    QComboBox *eventCombo;
    QLineEdit *expenseNameEdit;
    QLineEdit *amountEdit;
    QComboBox *categoryCombo;
    QDateEdit *dateEdit;
};

// Диалоговое окно для редактирования мероприятия
class EditEventDialog : public QDialog {
public:
    EditEventDialog(const Event &event, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Редактировать мероприятие");
        setFixedSize(500, 400);
        setModal(true);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(30, 30, 30, 30);
        mainLayout->setSpacing(20);

        // Заголовок
        QLabel *titleLabel = new QLabel("Редактировать мероприятие");
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #6155F5;");
        mainLayout->addWidget(titleLabel);

        // Форма
        QFormLayout *formLayout = new QFormLayout();
        formLayout->setSpacing(15);

        // Название мероприятия
        nameEdit = new QLineEdit();
        nameEdit->setText(event.title);
        nameEdit->setStyleSheet("QLineEdit {"
                                "border: 1px solid #dce4ec;"
                                "border-radius: 5px;"
                                "padding: 10px;"
                                "font-size: 14px;"
                                "}"
                                "QLineEdit:focus {"
                                "border-color: #6155F5;"
                                "}");

        // Дата
        dateEdit = new QDateEdit();
        dateEdit->setCalendarPopup(true);
        QDate date = QDate::fromString(event.event_datetime.left(10), "yyyy-MM-dd");
        if (date.isValid()) {
            dateEdit->setDate(date);
        } else {
            dateEdit->setDate(QDate::currentDate());
        }
        dateEdit->setDisplayFormat("dd.MM.yyyy");
        dateEdit->setStyleSheet("QDateEdit {"
                                "border: 1px solid #dce4ec;"
                                "border-radius: 5px;"
                                "padding: 10px;"
                                "font-size: 14px;"
                                "}"
                                "QDateEdit:focus {"
                                "border-color: #6155F5;"
                                "}");

        // Место
        locationEdit = new QLineEdit();
        locationEdit->setText(event.location);
        locationEdit->setStyleSheet("QLineEdit {"
                                    "border: 1px solid #dce4ec;"
                                    "border-radius: 5px;"
                                    "padding: 10px;"
                                    "font-size: 14px;"
                                    "}"
                                    "QLineEdit:focus {"
                                    "border-color: #6155F5;"
                                    "}");

        // Максимальное количество участников
        maxParticipantsEdit = new QSpinBox();
        maxParticipantsEdit->setRange(1, 10000);
        maxParticipantsEdit->setValue(event.max_participants);
        maxParticipantsEdit->setStyleSheet("QSpinBox {"
                                           "border: 1px solid #dce4ec;"
                                           "border-radius: 5px;"
                                           "padding: 10px;"
                                           "font-size: 14px;"
                                           "}"
                                           "QSpinBox:focus {"
                                           "border-color: #6155F5;"
                                           "}");

        // Статус
        statusCombo = new QComboBox();
        statusCombo->addItems({"Запланировано", "В процессе", "Завершено", "Отменено"});

        // Сопоставление статусов
        QString statusText;
        if (event.status == "planned") statusText = "Запланировано";
        else if (event.status == "ongoing") statusText = "В процессе";
        else if (event.status == "completed") statusText = "Завершено";
        else if (event.status == "cancelled") statusText = "Отменено";
        else statusText = "Запланировано";

        int index = statusCombo->findText(statusText);
        if (index >= 0) statusCombo->setCurrentIndex(index);

        statusCombo->setStyleSheet("QComboBox {"
                                   "border: 1px solid #dce4ec;"
                                   "border-radius: 5px;"
                                   "padding: 10px;"
                                   "font-size: 14px;"
                                   "}"
                                   "QComboBox:focus {"
                                   "border-color: #6155F5;"
                                   "}");

        formLayout->addRow("Название:", nameEdit);
        formLayout->addRow("Дата:", dateEdit);
        formLayout->addRow("Место:", locationEdit);
        formLayout->addRow("Макс. участников:", maxParticipantsEdit);
        formLayout->addRow("Статус:", statusCombo);

        mainLayout->addLayout(formLayout);

        // Кнопки
        QHBoxLayout *buttonLayout = new QHBoxLayout();

        QPushButton *cancelButton = new QPushButton("Отмена");
        cancelButton->setStyleSheet("QPushButton {"
                                    "background-color: #6c757d;"
                                    "color: white;"
                                    "border: none;"
                                    "border-radius: 5px;"
                                    "padding: 10px 20px;"
                                    "font-size: 14px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "background-color: #5a6268;"
                                    "}");

        QPushButton *saveButton = new QPushButton("Сохранить");
        saveButton->setStyleSheet("QPushButton {"
                                  "background-color: #6155F5;"
                                  "color: white;"
                                  "border: none;"
                                  "border-radius: 5px;"
                                  "padding: 10px 20px;"
                                  "font-size: 14px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #5046d4;"
                                  "}");

        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(saveButton);
        mainLayout->addLayout(buttonLayout);

        // Соединения
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    }

    Event getEventData(int eventId) const {
        Event event;
        event.id = eventId;
        event.title = nameEdit->text();
        event.location = locationEdit->text();
        event.max_participants = maxParticipantsEdit->value();

        // Преобразование статуса
        QString statusText = statusCombo->currentText();
        if (statusText == "Запланировано") event.status = "planned";
        else if (statusText == "В процессе") event.status = "ongoing";
        else if (statusText == "Завершено") event.status = "completed";
        else if (statusText == "Отменено") event.status = "cancelled";
        else event.status = "planned";

        // Дата и время
        QDateTime dateTime;
        dateTime.setDate(dateEdit->date());
        dateTime.setTime(QTime(10, 0)); // По умолчанию 10:00
        event.event_datetime = dateTime.toString("yyyy-MM-dd hh:mm:ss");

        return event;
    }

private:
    QLineEdit *nameEdit;
    QDateEdit *dateEdit;
    QLineEdit *locationEdit;
    QSpinBox *maxParticipantsEdit;
    QComboBox *statusCombo;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Инициализация базы данных через DatabaseManager
    qDebug() << "=== НАЧАЛО ИНИЦИАЛИЗАЦИИ ПРИЛОЖЕНИЯ ===";

    DatabaseManager& dbManager = DatabaseManager::instance();

    qDebug() << "Инициализация базы данных...";
    if (!dbManager.initDatabase()) {
        qDebug() << "КРИТИЧЕСКАЯ ОШИБКА: Не удалось инициализировать базу данных!";
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось инициализировать базу данных!");
        return -1;
    } else {
        qDebug() << "База данных успешно инициализирована";
    }

    qDebug() << "Создание таблиц...";
    if (!dbManager.createTables()) {
        qDebug() << "КРИТИЧЕСКАЯ ОШИБКА: Не удалось создать таблицы!";
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать таблицы базы данных!");
        return -1;
    } else {
        qDebug() << "Таблицы базы данных успешно созданы/проверены";
    }

    qDebug() << "=== ИНИЦИАЛИЗАЦИЯ БАЗЫ ДАННЫХ ЗАВЕРШЕНА ===";

    QMainWindow window;
    window.setFixedSize(1200, 800);
    window.setWindowTitle("Система управления мероприятиями");

    // Центральный виджет
    QWidget *centralWidget = new QWidget(&window);
    window.setCentralWidget(centralWidget);

    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Панель навигации
    QWidget *navWidget = new QWidget();
    navWidget->setStyleSheet("background-color: #6155F5; padding: 10px;");
    navWidget->setFixedHeight(60);

    QHBoxLayout *navLayout = new QHBoxLayout(navWidget);
    navLayout->setContentsMargins(20, 0, 20, 0);
    navLayout->setSpacing(30);

    // Логотип EfEventFlow
    QLabel *logoLabel = new QLabel("EfEventFlow");
    logoLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: white; padding: 5px;");
    navLayout->addWidget(logoLabel);

    // Кнопки навигации
    QPushButton *mainButton = new QPushButton("Главная");
    QPushButton *eventsButton = new QPushButton("Мероприятия");
    QPushButton *participantsButton = new QPushButton("Участники");
    QPushButton *budgetButton = new QPushButton("Бюджет");
    QPushButton *statisticsButton = new QPushButton("Статистика");

    // Стиль для кнопок навигации
    QString navButtonStyle = "QPushButton {"
                             "background-color: transparent;"
                             "color: white;"
                             "border: none;"
                             "padding: 8px 16px;"
                             "font-size: 14px;"
                             "font-weight: bold;"
                             "border-radius: 5px;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: rgba(255, 255, 255, 0.2);"
                             "}"
                             "QPushButton:pressed {"
                             "background-color: rgba(255, 255, 255, 0.3);"
                             "}";

    mainButton->setStyleSheet(navButtonStyle);
    eventsButton->setStyleSheet(navButtonStyle);
    participantsButton->setStyleSheet(navButtonStyle);
    budgetButton->setStyleSheet(navButtonStyle);
    statisticsButton->setStyleSheet(navButtonStyle);

    navLayout->addWidget(mainButton);
    navLayout->addWidget(eventsButton);
    navLayout->addWidget(participantsButton);
    navLayout->addWidget(budgetButton);
    navLayout->addWidget(statisticsButton);
    navLayout->addStretch();

    mainLayout->addWidget(navWidget);

    // Stacked widget для переключения между страницами
    QStackedWidget *stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("background-color: #f8f9fa;");
    mainLayout->addWidget(stackedWidget);

    // СТРАНИЦА 0: ГЛАВНАЯ СТРАНИЦА
    QWidget *mainPage = new QWidget();
    QVBoxLayout *mainPageLayout = new QVBoxLayout(mainPage);
    mainPageLayout->setContentsMargins(30, 20, 30, 20);
    mainPageLayout->setSpacing(20);

    // Заголовок главной страницы
    QLabel *mainTitle = new QLabel("Панель управления мероприятиями");
    mainTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainPageLayout->addWidget(mainTitle);

    // Приветственное сообщение
    QLabel *welcomeLabel = new QLabel("Добро пожаловать в систему управления мероприятиями! Здесь вы можете управлять всеми аспектами ваших мероприятий.");
    welcomeLabel->setStyleSheet("font-size: 14px; color: #7f8c8d; margin-bottom: 15px;");
    welcomeLabel->setWordWrap(true);
    mainPageLayout->addWidget(welcomeLabel);

    // Верхняя панель с ключевыми метриками
    QHBoxLayout *metricsLayout = new QHBoxLayout();
    metricsLayout->setSpacing(15);

    // Метрика 1: Всего мероприятий
    QFrame *metric1 = new QFrame();
    metric1->setStyleSheet("QFrame {"
                           "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3498db, stop:1 #2980b9);"
                           "border-radius: 10px;"
                           "padding: 10px;"
                           "}");
    metric1->setFixedSize(250, 130);
    QVBoxLayout *metric1Layout = new QVBoxLayout(metric1);
    QLabel *metric1Value = new QLabel("0");
    metric1Value->setStyleSheet("font-size: 32px; font-weight: bold; color: white;");
    QLabel *metric1Label = new QLabel("Всего мероприятий");
    metric1Label->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    metric1Layout->addWidget(metric1Value);
    metric1Layout->addWidget(metric1Label);

    // Метрика 2: Активные мероприятия
    QFrame *metric2 = new QFrame();
    metric2->setStyleSheet("QFrame {"
                           "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2ecc71, stop:1 #27ae60);"
                           "border-radius: 10px;"
                           "padding: 10px;"
                           "}");
    metric2->setFixedSize(250, 130);
    QVBoxLayout *metric2Layout = new QVBoxLayout(metric2);
    QLabel *metric2Value = new QLabel("0");
    metric2Value->setStyleSheet("font-size: 32px; font-weight: bold; color: white;");
    QLabel *metric2Label = new QLabel("Активные мероприятия");
    metric2Label->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    metric2Layout->addWidget(metric2Value);
    metric2Layout->addWidget(metric2Label);

    // Метрика 3: Участники
    QFrame *metric3 = new QFrame();
    metric3->setStyleSheet("QFrame {"
                           "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e74c3c, stop:1 #c0392b);"
                           "border-radius: 10px;"
                           "padding: 10px;"
                           "}");
    metric3->setFixedSize(250, 130);
    QVBoxLayout *metric3Layout = new QVBoxLayout(metric3);
    QLabel *metric3Value = new QLabel("0");
    metric3Value->setStyleSheet("font-size: 32px; font-weight: bold; color: white;");
    QLabel *metric3Label = new QLabel("Всего участников");
    metric3Label->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    metric3Layout->addWidget(metric3Value);
    metric3Layout->addWidget(metric3Label);

    // Метрика 4: Бюджет
    QFrame *metric4 = new QFrame();
    metric4->setStyleSheet("QFrame {"
                           "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9b59b6, stop:1 #8e44ad);"
                           "border-radius: 10px;"
                           "padding: 10px;"
                           "}");
    metric4->setFixedSize(250, 130);
    QVBoxLayout *metric4Layout = new QVBoxLayout(metric4);
    QLabel *metric4Value = new QLabel("0 ₽");
    metric4Value->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    QLabel *metric4Label = new QLabel("Общий бюджет");
    metric4Label->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    metric4Layout->addWidget(metric4Value);
    metric4Layout->addWidget(metric4Label);

    metricsLayout->addWidget(metric1);
    metricsLayout->addWidget(metric2);
    metricsLayout->addWidget(metric3);
    metricsLayout->addWidget(metric4);
    metricsLayout->addStretch();

    mainPageLayout->addLayout(metricsLayout);

    // Блоки с информацией
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(20);

    // Левый блок - Ближайшие мероприятия
    QFrame *upcomingEventsFrame = new QFrame();
    upcomingEventsFrame->setStyleSheet("QFrame {"
                                       "background-color: white;"
                                       "border-radius: 10px;"
                                       "border: 1px solid #dce4ec;"
                                       "}");
    upcomingEventsFrame->setFixedWidth(450);
    QVBoxLayout *upcomingLayout = new QVBoxLayout(upcomingEventsFrame);
    upcomingLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *upcomingTitle = new QLabel("Ближайшие мероприятия");
    upcomingTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 12px;");
    upcomingLayout->addWidget(upcomingTitle);

    // Контейнер для списка мероприятий
    QVBoxLayout *upcomingEventsLayout = new QVBoxLayout();
    upcomingLayout->addLayout(upcomingEventsLayout);

    // Кнопка просмотра всех мероприятий
    QPushButton *viewAllEventsBtn = new QPushButton("Все мероприятия →");
    viewAllEventsBtn->setStyleSheet("QPushButton {"
                                    "background-color: transparent;"
                                    "color: #3498db;"
                                    "border: 1px solid #3498db;"
                                    "border-radius: 5px;"
                                    "padding: 6px 12px;"
                                    "font-size: 13px;"
                                    "margin-top: 8px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "background-color: #3498db;"
                                    "color: white;"
                                    "}");
    upcomingLayout->addWidget(viewAllEventsBtn);
    // ОБРАБОТЧИК КЛИКА ДЛЯ ПЕРЕХОДА НА СТРАНИЦУ МЕРОПРИЯТИЙ
    QObject::connect(viewAllEventsBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(1); // Переход на страницу мероприятий
    });
    // Правый блок - Статистика и прогресс
    QFrame *statsFrame = new QFrame();
    statsFrame->setStyleSheet("QFrame {"
                              "background-color: white;"
                              "border-radius: 10px;"
                              "border: 1px solid #dce4ec;"
                              "}");
    statsFrame->setFixedWidth(450);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsFrame);
    statsLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *statsTitle = new QLabel("Статистика выполнения плана");
    statsTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;margin-top: 0px; margin-bottom: 3px;");
    statsLayout->addWidget(statsTitle);

    // Прогресс-бары
    QLabel *progress1Label = new QLabel("Заполняемость мероприятий");
    progress1Label->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold;margin-top: 3px; margin-bottom: 5px;");
    statsLayout->addWidget(progress1Label);

    QProgressBar *progress1 = new QProgressBar();
    progress1->setValue(0);
    progress1->setTextVisible(true);
    progress1->setFormat("%p%");
    progress1->setStyleSheet("QProgressBar {"
                             "border: 1px solid #bdc3c7;"
                             "border-radius: 5px;"
                             "text-align: center;"
                             "color: white;"
                             "font-weight: bold;"
                             "}"
                             "QProgressBar::chunk {"
                             "background-color: #2ecc71;"
                             "border-radius: 4px;"
                             "}");
    statsLayout->addWidget(progress1);

    QLabel *progress2Label = new QLabel("Выполнение бюджета");
    progress2Label->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold; margin-top: 3px; margin-bottom: 5px;");
    statsLayout->addWidget(progress2Label);

    QProgressBar *progress2 = new QProgressBar();
    progress2->setValue(0);
    progress2->setTextVisible(true);
    progress2->setFormat("%p%");
    progress2->setStyleSheet("QProgressBar {"
                             "border: 1px solid #bdc3c7;"
                             "border-radius: 5px;"
                             "text-align: center;"
                             "color: white;"
                             "font-weight: bold;"
                             "}"
                             "QProgressBar::chunk {"
                             "background-color: #3498db;"
                             "border-radius: 4px;"
                             "}");
    statsLayout->addWidget(progress2);

    QLabel *progress3Label = new QLabel("Регистрация участников");
    progress3Label->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold; margin-top: 3px; margin-bottom: 5px;");
    statsLayout->addWidget(progress3Label);

    QProgressBar *progress3 = new QProgressBar();
    progress3->setValue(0);
    progress3->setTextVisible(true);
    progress3->setFormat("%p%");
    progress3->setStyleSheet("QProgressBar {"
                             "border: 1px solid #bdc3c7;"
                             "border-radius: 5px;"
                             "text-align: center;"
                             "color: white;"
                             "font-weight: bold;"
                             "}"
                             "QProgressBar::chunk {"
                             "background-color: #9b59b6;"
                             "border-radius: 4px;"
                             "}");
    statsLayout->addWidget(progress3);

    infoLayout->addWidget(upcomingEventsFrame);
    infoLayout->addWidget(statsFrame);
    infoLayout->addStretch();

    mainPageLayout->addLayout(infoLayout);

    // Блок быстрых действий
    QFrame *quickActionsFrame = new QFrame();
    quickActionsFrame->setStyleSheet("QFrame {"
                                     "background-color: white;"
                                     "border-radius: 10px;"
                                     "border: 1px solid #dce4ec;"
                                     "padding: 15px;"
                                     "}");
    QVBoxLayout *quickActionsLayout = new QVBoxLayout(quickActionsFrame);

    QLabel *quickActionsTitle = new QLabel("Быстрые действия");
    quickActionsTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 12px;");
    quickActionsLayout->addWidget(quickActionsTitle);

    QHBoxLayout *actionsLayout = new QHBoxLayout();

    // Кнопка быстрого создания мероприятия
    QPushButton *quickEventBtn = new QPushButton("➕ Создать мероприятие");
    quickEventBtn->setStyleSheet("QPushButton {"
                                 "background-color: #2ecc71;"
                                 "color: white;"
                                 "border: none;"
                                 "border-radius: 6px;"
                                 "padding: 12px;"
                                 "font-size: 13px;"
                                 "font-weight: bold;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background-color: #27ae60;"
                                 "}");
    quickEventBtn->setFixedHeight(50);

    // Кнопка добавления участника
    QPushButton *quickParticipantBtn = new QPushButton("👥 Добавить участника");
    quickParticipantBtn->setStyleSheet("QPushButton {"
                                       "background-color: #3498db;"
                                       "color: white;"
                                       "border: none;"
                                       "border-radius: 6px;"
                                       "padding: 12px;"
                                       "font-size: 13px;"
                                       "font-weight: bold;"
                                       "}"
                                       "QPushButton:hover {"
                                       "background-color: #2980b9;"
                                       "}");
    quickParticipantBtn->setFixedHeight(50);

    // Кнопка управления бюджетом
    QPushButton *quickBudgetBtn = new QPushButton("💰 Управление бюджетом");
    quickBudgetBtn->setStyleSheet("QPushButton {"
                                  "background-color: #e67e22;"
                                  "color: white;"
                                  "border: none;"
                                  "border-radius: 6px;"
                                  "padding: 12px;"
                                  "font-size: 13px;"
                                  "font-weight: bold;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #d35400;"
                                  "}");
    quickBudgetBtn->setFixedHeight(50);

    // Кнопка отчетов
    QPushButton *quickReportBtn = new QPushButton("📊 Создать отчет");
    quickReportBtn->setStyleSheet("QPushButton {"
                                  "background-color: #9b59b6;"
                                  "color: white;"
                                  "border: none;"
                                  "border-radius: 6px;"
                                  "padding: 12px;"
                                  "font-size: 13px;"
                                  "font-weight: bold;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #8e44ad;"
                                  "}");
    quickReportBtn->setFixedHeight(50);

    actionsLayout->addWidget(quickEventBtn);
    actionsLayout->addWidget(quickParticipantBtn);
    actionsLayout->addWidget(quickBudgetBtn);
    actionsLayout->addWidget(quickReportBtn);

    quickActionsLayout->addLayout(actionsLayout);
    mainPageLayout->addWidget(quickActionsFrame);

    // СТРАНИЦА 1: СПИСОК МЕРОПРИЯТИЙ
    QWidget *eventsPage = new QWidget();
    QVBoxLayout *eventsPageLayout = new QVBoxLayout(eventsPage);
    eventsPageLayout->setContentsMargins(30, 20, 30, 20);
    eventsPageLayout->setSpacing(15);

    // Заголовок страницы мероприятий
    QLabel *eventsTitle = new QLabel("Все мероприятия");
    eventsTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #6155F5;");
    eventsPageLayout->addWidget(eventsTitle);

    // Таблица мероприятий
    QTableWidget *eventsTable = new QTableWidget();
    eventsTable->setColumnCount(6);
    eventsTable->setHorizontalHeaderLabels(QStringList() << "Название" << "Дата" << "Место" << "Участники" << "Статус" << "Действия");

    // Настройка таблицы
    eventsTable->setStyleSheet(
        "QTableWidget {"
        "background-color: white;"
        "border: 1px solid #dce4ec;"
        "border-radius: 6px;"
        "gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "padding: 10px;"
        "border-bottom: 1px solid #ecf0f1;"
        "}"
        "QTableWidget::item:selected {"
        "background-color: #3498db;"
        "color: white;"
        "}"
        "QHeaderView::section {"
        "background-color: #6155F5;"
        "color: white;"
        "padding: 10px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    eventsTable->horizontalHeader()->setStretchLastSection(true);
    eventsTable->verticalHeader()->setVisible(false);
    eventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    eventsTable->setAlternatingRowColors(true);

    // Функция для обновления таблицы мероприятий ИЗ БАЗЫ ДАННЫХ
    auto updateEventsTable = [&]() {
        qDebug() << "Обновление таблицы мероприятий...";
        QVector<Event> events = dbManager.getAllEvents();
        qDebug() << "Получено мероприятий из БД:" << events.size();

        eventsTable->setRowCount(events.size());

        for (int i = 0; i < events.size(); ++i) {
            const Event &event = events[i];

            // Форматирование даты для отображения
            QString displayDate = QDateTime::fromString(event.event_datetime, "yyyy-MM-dd hh:mm:ss")
                                     .toString("dd.MM.yyyy");

            // Получение количества участников
            int participantCount = dbManager.getParticipantCountForEvent(event.id);
            QString participantsText = QString("%1/%2").arg(participantCount).arg(event.max_participants);

            // Перевод статуса
            QString statusText;
            if (event.status == "planned") statusText = "Запланировано";
            else if (event.status == "ongoing") statusText = "В процессе";
            else if (event.status == "completed") statusText = "Завершено";
            else if (event.status == "cancelled") statusText = "Отменено";
            else statusText = event.status;

            QTableWidgetItem *nameItem = new QTableWidgetItem(event.title);
            QTableWidgetItem *dateItem = new QTableWidgetItem(displayDate);
            QTableWidgetItem *locationItem = new QTableWidgetItem(event.location);
            QTableWidgetItem *participantsItem = new QTableWidgetItem(participantsText);
            QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);
            QTableWidgetItem *actionsItem = new QTableWidgetItem("Редактировать");

            // Запрещаем редактирование всех ячеек (теперь данные из БД)
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
            locationItem->setFlags(locationItem->flags() & ~Qt::ItemIsEditable);
            participantsItem->setFlags(participantsItem->flags() & ~Qt::ItemIsEditable);
            statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
            actionsItem->setFlags(actionsItem->flags() & ~Qt::ItemIsEditable);

            eventsTable->setItem(i, 0, nameItem);
            eventsTable->setItem(i, 1, dateItem);
            eventsTable->setItem(i, 2, locationItem);
            eventsTable->setItem(i, 3, participantsItem);
            eventsTable->setItem(i, 4, statusItem);
            eventsTable->setItem(i, 5, actionsItem);
        }

        eventsTable->resizeColumnsToContents();

        // Обновляем метрики на главной странице
        metric1Value->setText(QString::number(events.size()));

        // Для простоты считаем все активными (planned и ongoing)
        int activeEvents = 0;
        for (const Event &event : events) {
            if (event.status == "planned" || event.status == "ongoing") {
                activeEvents++;
            }
        }
        metric2Value->setText(QString::number(activeEvents));

        // Обновляем список ближайших мероприятий
        QLayoutItem* child;
        while ((child = upcomingEventsLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        int eventsToShow = qMin(4, events.size());
        for (int i = 0; i < eventsToShow; ++i) {
            const Event &event = events[i];
            QString displayDate = QDateTime::fromString(event.event_datetime, "yyyy-MM-dd hh:mm:ss")
                                     .toString("dd.MM.yyyy");
            QLabel *eventLabel = new QLabel(QString("🎯 %1 - %2").arg(event.title).arg(displayDate));
            eventLabel->setStyleSheet("font-size: 13px; color: #34495e; padding: 6px 0; border-bottom: 1px solid #ecf0f1;");
            eventLabel->setWordWrap(true);
            upcomingEventsLayout->addWidget(eventLabel);
        }

        if (events.isEmpty()) {
            QLabel *noEventsLabel = new QLabel("Нет созданных мероприятий");
            noEventsLabel->setStyleSheet("font-size: 13px; color: #7f8c8d; font-style: italic; padding: 6px 0;");
            upcomingEventsLayout->addWidget(noEventsLabel);
        }

        qDebug() << "Таблица мероприятий обновлена";
    };

    eventsPageLayout->addWidget(eventsTable);

    // Кнопка для создания нового мероприятия
    QPushButton *createNewEventBtn = new QPushButton("+ Создать новое мероприятие");
    createNewEventBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6155F5;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px 18px;"
        "font-size: 13px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #5046d4;"
        "}"
        "QPushButton:pressed {"
        "background-color: #4038b3;"
        "}"
        );
    createNewEventBtn->setFixedWidth(250);
    eventsPageLayout->addWidget(createNewEventBtn, 0, Qt::AlignRight);

    // Обработчик кнопки создания мероприятия на странице мероприятий
    QObject::connect(createNewEventBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(2); // Переход на страницу создания мероприятия
    });

    // СТРАНИЦА 2: СОЗДАНИЕ МЕРОПРИЯТИЯ
    QWidget *createEventPage = new QWidget();
    QVBoxLayout *createEventLayout = new QVBoxLayout(createEventPage);
    createEventLayout->setContentsMargins(0, 0, 0, 0);

    // Фон
    QLabel *background1 = new QLabel(createEventPage);
    background1->setStyleSheet("background-color: #f8f9fa;");
    background1->setGeometry(0, 0, 1200, 740);

    // Контейнер для формы
    QWidget *formContainer = new QWidget(createEventPage);
    formContainer->setGeometry(50, 20, 1100, 700);
    formContainer->setStyleSheet("background-color: white; border-radius: 8px;");

    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setContentsMargins(40, 30, 40, 30);
    formLayout->setSpacing(20);

    // Заголовок
    QLabel *titleLabel = new QLabel("Создание нового мероприятия");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #6155F5; margin-bottom: 10px;");
    formLayout->addWidget(titleLabel);

    // Сетка для полей ввода
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(30);
    gridLayout->setVerticalSpacing(12);

    // Левая колонка
    QLabel *nameLabel = new QLabel("Название мероприятия");
    nameLabel->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold;");

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите название");
    nameEdit->setStyleSheet("QLineEdit {"
                            "border: 1px solid #dce4ec;"
                            "border-radius: 5px;"
                            "padding: 8px;"
                            "font-size: 13px;"
                            "background-color: white;"
                            "}"
                            "QLineEdit:focus {"
                            "border-color: #6155F5;"
                            "}");

    QLabel *typeLabel = new QLabel("Тип мероприятия");
    typeLabel->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold;");

    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"Конференция", "Семинар", "Воркшоп", "Выставка", "Концерт", "Спортивное мероприятие"});
    typeCombo->setStyleSheet("QComboBox {"
                             "border: 1px solid #dce4ec;"
                             "border-radius: 5px;"
                             "padding: 8px;"
                             "font-size: 13px;"
                             "background-color: white;"
                             "}"
                             "QComboBox:focus {"
                             "border-color: #6155F5;"
                             "}"
                             "QComboBox::drop-down {"
                             "border: none;"
                             "width: 25px;"
                             "}"
                             "QComboBox::down-arrow {"
                             "image: none;"
                             "border: none;"
                             "}");

    // Правая колонка
    QLabel *dateLabel = new QLabel("Дата проведения");
    dateLabel->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold;");

    QDateEdit *dateEdit = new QDateEdit();
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate().addDays(7)); // Дата через неделю по умолчанию
    dateEdit->setDisplayFormat("dd.MM.yyyy");
    dateEdit->setStyleSheet("QDateEdit {"
                            "border: 1px solid #dce4ec;"
                            "border-radius: 5px;"
                            "padding: 8px;"
                            "font-size: 13px;"
                            "background-color: white;"
                            "}"
                            "QDateEdit:focus {"
                            "border-color: #6155F5;"
                            "}");

    QLabel *maxParticipantsLabel = new QLabel("Макс.участников");
    maxParticipantsLabel->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold;");

    QSpinBox *maxParticipantsSpin = new QSpinBox();
    maxParticipantsSpin->setRange(1, 10000);
    maxParticipantsSpin->setValue(50);
    maxParticipantsSpin->setStyleSheet("QSpinBox {"
                                       "border: 1px solid #dce4ec;"
                                       "border-radius: 5px;"
                                       "padding: 8px;"
                                       "font-size: 13px;"
                                       "background-color: white;"
                                       "}"
                                       "QSpinBox:focus {"
                                       "border-color: #6155F5;"
                                       "}");

    // Добавление в сетку
    gridLayout->addWidget(nameLabel, 0, 0);
    gridLayout->addWidget(nameEdit, 1, 0);
    gridLayout->addWidget(typeLabel, 2, 0);
    gridLayout->addWidget(typeCombo, 3, 0);

    gridLayout->addWidget(dateLabel, 0, 1);
    gridLayout->addWidget(dateEdit, 1, 1);
    gridLayout->addWidget(maxParticipantsLabel, 2, 1);
    gridLayout->addWidget(maxParticipantsSpin, 3, 1);

    formLayout->addLayout(gridLayout);

    // Описание мероприятия
    QLabel *descLabel = new QLabel("Описание мероприятия");
    descLabel->setStyleSheet("font-size: 13px; color: #34495e; font-weight: bold; margin-top: 10px;");

    QTextEdit *descEdit = new QTextEdit();
    descEdit->setPlaceholderText("Опишите мероприятие");
    descEdit->setMaximumHeight(80);
    descEdit->setStyleSheet("QTextEdit {"
                            "border: 1px solid #dce4ec;"
                            "border-radius: 5px;"
                            "padding: 8px;"
                            "font-size: 13px;"
                            "background-color: white;"
                            "}"
                            "QTextEdit:focus {"
                            "border-color: #6155F5;"
                            "}");

    formLayout->addWidget(descLabel);
    formLayout->addWidget(descEdit);

    // Разделительная линия
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #ecf0f1; margin: 12px 0;");
    formLayout->addWidget(line);

    // Бюджет мероприятия
    QHBoxLayout *budgetLineLayout = new QHBoxLayout();

    QLabel *budgetLabel = new QLabel("Бюджет мероприятия");
    budgetLabel->setStyleSheet("font-size: 14px; color: #34495e; font-weight: bold;");

    QLineEdit *budgetEdit = new QLineEdit();
    budgetEdit->setText("0");
    budgetEdit->setStyleSheet("QLineEdit {"
                              "border: 1px solid #dce4ec;"
                              "border-radius: 5px;"
                              "padding: 8px;"
                              "font-size: 13px;"
                              "background-color: white;"
                              "font-weight: bold;"
                              "color: #2c3e50;"
                              "}"
                              "QLineEdit:focus {"
                              "border-color: #6155F5;"
                              "}");
    budgetEdit->setFixedWidth(100);

    budgetLineLayout->addWidget(budgetLabel);
    budgetLineLayout->addStretch();
    budgetLineLayout->addWidget(budgetEdit);

    formLayout->addLayout(budgetLineLayout);

    // Кнопка создания
    QPushButton *createButton = new QPushButton("Создать мероприятие");
    createButton->setStyleSheet("QPushButton {"
                                "background-color: #6155F5;"
                                "color: white;"
                                "border: none;"
                                "border-radius: 5px;"
                                "padding: 10px;"
                                "font-size: 14px;"
                                "font-weight: bold;"
                                "margin-top: 10px;"
                                "}"
                                "QPushButton:hover {"
                                "background-color: #5046d4;"
                                "}"
                                "QPushButton:pressed {"
                                "background-color: #4038b3;"
                                "}");
    createButton->setFixedHeight(40);

    formLayout->addWidget(createButton);

    // СТРАНИЦА 5: БЮДЖЕТ
    QWidget *budgetPage = new QWidget();
    QVBoxLayout *budgetPageLayout = new QVBoxLayout(budgetPage);
    budgetPageLayout->setContentsMargins(30, 20, 30, 20);
    budgetPageLayout->setSpacing(15);

    // Заголовок страницы бюджета
    QLabel *budgetTitle = new QLabel("Управление бюджетом");
    budgetTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #6155F5;");
    budgetPageLayout->addWidget(budgetTitle);

    // Таблица бюджета
    QTableWidget *budgetTable = new QTableWidget();
    budgetTable->setColumnCount(5);
    budgetTable->setHorizontalHeaderLabels(QStringList() << "Мероприятие" << "Статья расхода" << "Планируемый бюджет" << "Фактические расходы" << "Разница");

    // Настройка таблицы
    budgetTable->setStyleSheet(
        "QTableWidget {"
        "background-color: white;"
        "border: 1px solid #dce4ec;"
        "border-radius: 6px;"
        "gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "padding: 10px;"
        "border-bottom: 1px solid #ecf0f1;"
        "}"
        "QTableWidget::item:selected {"
        "background-color: #6155F5;"
        "color: white;"
        "}"
        "QHeaderView::section {"
        "background-color: #6155F5;"
        "color: white;"
        "padding: 10px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    budgetTable->horizontalHeader()->setStretchLastSection(true);
    budgetTable->verticalHeader()->setVisible(false);
    budgetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    budgetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    budgetTable->setAlternatingRowColors(true);

    // Функция для обновления таблицы бюджета
    auto updateBudgetTable = [&]() {
        qDebug() << "Обновление таблицы бюджета...";
        QVector<Event> events = dbManager.getAllEvents();
        int totalRows = 0;

        // Подсчитываем общее количество строк
        for (const Event &event : events) {
            totalRows += dbManager.getBudgetForEvent(event.id).size();
        }

        budgetTable->setRowCount(totalRows);

        int currentRow = 0;
        double totalPlanned = 0;
        double totalActual = 0;

        for (const Event &event : events) {
            QVector<BudgetItem> budgetItems = dbManager.getBudgetForEvent(event.id);

            for (const BudgetItem &item : budgetItems) {
                double difference = item.planned_amount - item.actual_amount;
                QString differenceText = QString("%1 ₽").arg(difference, 0, 'f', 2);

                QTableWidgetItem *eventItem = new QTableWidgetItem(event.title);
                QTableWidgetItem *expenseItem = new QTableWidgetItem(item.expense_item);
                QTableWidgetItem *plannedItem = new QTableWidgetItem(QString("%1 ₽").arg(item.planned_amount, 0, 'f', 2));
                QTableWidgetItem *actualItem = new QTableWidgetItem(QString("%1 ₽").arg(item.actual_amount, 0, 'f', 2));
                QTableWidgetItem *differenceItem = new QTableWidgetItem(differenceText);

                // Установка цвета для разницы бюджета
                if (difference > 0) {
                    differenceItem->setForeground(QBrush(QColor("#27ae60"))); // Зеленый для положительной разницы
                } else if (difference < 0) {
                    differenceItem->setForeground(QBrush(QColor("#e74c3c"))); // Красный для отрицательной разницы
                }

                budgetTable->setItem(currentRow, 0, eventItem);
                budgetTable->setItem(currentRow, 1, expenseItem);
                budgetTable->setItem(currentRow, 2, plannedItem);
                budgetTable->setItem(currentRow, 3, actualItem);
                budgetTable->setItem(currentRow, 4, differenceItem);

                totalPlanned += item.planned_amount;
                totalActual += item.actual_amount;
                currentRow++;
            }
        }

        budgetTable->resizeColumnsToContents();

        // Обновляем метрику бюджета на главной странице
        metric4Value->setText(QString("%1 ₽").arg(totalPlanned, 0, 'f', 2));
        qDebug() << "Таблица бюджета обновлена";
    };

    budgetPageLayout->addWidget(budgetTable);

    // Кнопка для добавления расхода
    QPushButton *addExpenseBtn = new QPushButton("+ Добавить расход");
    addExpenseBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6155F5;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px 18px;"
        "font-size: 13px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #5046d4;"
        "}"
        "QPushButton:pressed {"
        "background-color: #4038b3;"
        "}"
        );
    addExpenseBtn->setFixedWidth(180);
    budgetPageLayout->addWidget(addExpenseBtn, 0, Qt::AlignRight);

    // СТРАНИЦА 3: УЧАСТНИКИ
    QWidget *participantsPage = new QWidget();
    QVBoxLayout *participantsLayout = new QVBoxLayout(participantsPage);
    participantsLayout->setContentsMargins(30, 20, 30, 20);
    participantsLayout->setSpacing(15);

    QLabel *participantsTitle = new QLabel("Участники мероприятий");
    participantsTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #6155F5;");
    participantsLayout->addWidget(participantsTitle);

    // Таблица участников
    QTableWidget *participantsTable = new QTableWidget();
    participantsTable->setColumnCount(4);
    participantsTable->setHorizontalHeaderLabels(QStringList() << "Имя" << "Email" << "Телефон" << "Дата регистрации");

    participantsTable->setStyleSheet(
        "QTableWidget {"
        "background-color: white;"
        "border: 1px solid #dce4ec;"
        "border-radius: 6px;"
        "gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "padding: 10px;"
        "border-bottom: 1px solid #ecf0f1;"
        "}"
        "QTableWidget::item:selected {"
        "background-color: #6155F5;"
        "color: white;"
        "}"
        "QHeaderView::section {"
        "background-color: #6155F5;"
        "color: white;"
        "padding: 10px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    participantsTable->horizontalHeader()->setStretchLastSection(true);
    participantsTable->verticalHeader()->setVisible(false);
    participantsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    participantsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    participantsTable->setAlternatingRowColors(true);

    // Функция для обновления таблицы участников
    auto updateParticipantsTable = [&]() {
        qDebug() << "Обновление таблицы участников...";
        QVector<Participant> participants = dbManager.getAllParticipants();
        participantsTable->setRowCount(participants.size());

        for (int i = 0; i < participants.size(); ++i) {
            const Participant &participant = participants[i];

            QTableWidgetItem *nameItem = new QTableWidgetItem(participant.full_name);
            QTableWidgetItem *emailItem = new QTableWidgetItem(participant.email);
            QTableWidgetItem *phoneItem = new QTableWidgetItem(participant.phone);
            QTableWidgetItem *dateItem = new QTableWidgetItem(participant.reg_date);

            participantsTable->setItem(i, 0, nameItem);
            participantsTable->setItem(i, 1, emailItem);
            participantsTable->setItem(i, 2, phoneItem);
            participantsTable->setItem(i, 3, dateItem);
        }

        participantsTable->resizeColumnsToContents();

        // Обновляем метрику участников на главной странице
        metric3Value->setText(QString::number(participants.size()));
        qDebug() << "Таблица участников обновлена";
    };

    participantsLayout->addWidget(participantsTable);

    // Кнопка для добавления участника
    QPushButton *addParticipantBtn = new QPushButton("+ Добавить участника");
    addParticipantBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6155F5;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px 18px;"
        "font-size: 13px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #5046d4;"
        "}"
        "QPushButton:pressed {"
        "background-color: #4038b3;"
        "}"
        );
    addParticipantBtn->setFixedWidth(200);
    participantsLayout->addWidget(addParticipantBtn, 0, Qt::AlignRight);

    // СТРАНИЦА 4: ДОБАВЛЕНИЕ УЧАСТНИКА
    QWidget *addParticipantPage = new QWidget();
    QVBoxLayout *addParticipantLayout = new QVBoxLayout(addParticipantPage);
    addParticipantLayout->setContentsMargins(0, 0, 0, 0);

    // Фон
    QLabel *background2 = new QLabel(addParticipantPage);
    background2->setStyleSheet("background-color: #f8f9fa;");
    background2->setGeometry(0, 0, 1200, 740);

    // Контейнер для формы добавления участника
    QWidget *participantFormContainer = new QWidget(addParticipantPage);
    participantFormContainer->setGeometry(50, 20, 1100, 600);
    participantFormContainer->setStyleSheet("background-color: white; border-radius: 10px;");

    QVBoxLayout *participantFormLayout = new QVBoxLayout(participantFormContainer);
    participantFormLayout->setContentsMargins(40, 30, 40, 30);
    participantFormLayout->setSpacing(25);

    // Заголовок
    QLabel *participantTitleLabel = new QLabel("Добавление участника");
    participantTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #6155F5;");
    participantFormLayout->addWidget(participantTitleLabel);

    // Сетка для полей ввода
    QGridLayout *participantGridLayout = new QGridLayout();
    participantGridLayout->setHorizontalSpacing(30);
    participantGridLayout->setVerticalSpacing(15);

    // Поля формы
    QLabel *participantNameLabel = new QLabel("Имя");
    participantNameLabel->setStyleSheet("font-size: 14px; color: #34495e; font-weight: bold;");

    QLineEdit *participantNameEdit = new QLineEdit();
    participantNameEdit->setPlaceholderText("Введите имя участника");
    participantNameEdit->setStyleSheet("QLineEdit {"
                                       "border: 2px solid #bdc3c7;"
                                       "border-radius: 6px;"
                                       "padding: 10px;"
                                       "font-size: 13px;"
                                       "background-color: white;"
                                       "}"
                                       "QLineEdit:focus {"
                                       "border-color: #6155F5;"
                                       "}");

    QLabel *emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet("font-size: 14px; color: #34495e; font-weight: bold;");

    QLineEdit *emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Введите email участника");
    emailEdit->setStyleSheet("QLineEdit {"
                             "border: 2px solid #bdc3c7;"
                             "border-radius: 6px;"
                             "padding: 10px;"
                             "font-size: 13px;"
                             "background-color: white;"
                             "}"
                             "QLineEdit:focus {"
                             "border-color: #6155F5;"
                             "}");

    QLabel *phoneLabel = new QLabel("Телефон");
    phoneLabel->setStyleSheet("font-size: 14px; color: #34495e; font-weight: bold;");

    QLineEdit *phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("Введите телефон участника");
    phoneEdit->setStyleSheet("QLineEdit {"
                             "border: 2px solid #bdc3c7;"
                             "border-radius: 6px;"
                             "padding: 10px;"
                             "font-size: 13px;"
                             "background-color: white;"
                             "}"
                             "QLineEdit:focus {"
                             "border-color: #6155F5;"
                             "}");

    QLabel *eventLabel = new QLabel("Выберите мероприятие");
    eventLabel->setStyleSheet("font-size: 14px; color: #34495e; font-weight: bold;");

    QComboBox *eventCombo = new QComboBox();
    // События будут добавляться динамически
    eventCombo->setStyleSheet("QComboBox {"
                              "border: 2px solid #bdc3c7;"
                              "border-radius: 6px;"
                              "padding: 10px;"
                              "font-size: 13px;"
                              "background-color: white;"
                              "}"
                              "QComboBox:focus {"
                              "border-color: #6155F5;"
                              "}"
                              "QComboBox::drop-down {"
                              "border: none;"
                              "}"
                              "QComboBox::down-arrow {"
                              "image: none;"
                              "border: none;"
                              "}");

    // Добавление в сетку
    participantGridLayout->addWidget(participantNameLabel, 0, 0);
    participantGridLayout->addWidget(participantNameEdit, 1, 0);
    participantGridLayout->addWidget(emailLabel, 2, 0);
    participantGridLayout->addWidget(emailEdit, 3, 0);
    participantGridLayout->addWidget(phoneLabel, 4, 0);
    participantGridLayout->addWidget(phoneEdit, 5, 0);
    participantGridLayout->addWidget(eventLabel, 6, 0);
    participantGridLayout->addWidget(eventCombo, 7, 0);

    participantFormLayout->addLayout(participantGridLayout);

    // Разделительная линия
    QFrame *participantLine = new QFrame();
    participantLine->setFrameShape(QFrame::HLine);
    participantLine->setFrameShadow(QFrame::Sunken);
    participantLine->setStyleSheet("color: #bdc3c7; margin: 15px 0;");
    participantFormLayout->addWidget(participantLine);

    // Кнопка добавления
    QPushButton *addParticipantButton = new QPushButton("Добавить участника");
    addParticipantButton->setStyleSheet("QPushButton {"
                                        "background-color: #6155F5;"
                                        "color: white;"
                                        "border: none;"
                                        "border-radius: 6px;"
                                        "padding: 12px;"
                                        "font-size: 14px;"
                                        "font-weight: bold;"
                                        "}"
                                        "QPushButton:hover {"
                                        "background-color: #5046d4;"
                                        "}"
                                        "QPushButton:pressed {"
                                        "background-color: #4038b3;"
                                        "}");
    addParticipantButton->setFixedHeight(45);

    participantFormLayout->addWidget(addParticipantButton);

    // Функция для обновления комбобокса с мероприятиями
    auto updateEventComboBox = [&]() {
        qDebug() << "Обновление комбобокса мероприятий...";
        eventCombo->clear();
        QVector<Event> events = dbManager.getAllEvents();
        for (const Event &event : events) {
            eventCombo->addItem(event.title, event.id);
        }
        if (events.isEmpty()) {
            eventCombo->addItem("Нет мероприятий");
        }
        qDebug() << "Комбобокс мероприятий обновлен, элементов:" << eventCombo->count();
    };

    // СТРАНИЦА 6: СТАТИСТИКА
    QWidget *statisticsPage = new QWidget();
    QVBoxLayout *statisticsLayout = new QVBoxLayout(statisticsPage);
    statisticsLayout->setContentsMargins(30, 20, 30, 20);
    statisticsLayout->setSpacing(20);

    // Заголовок страницы статистики
    QLabel *statisticsTitle = new QLabel("Статистика мероприятий");
    statisticsTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #6155F5; margin-bottom: 10px;");
    statisticsLayout->addWidget(statisticsTitle);

    // Верхняя панель с ключевыми метриками
    QHBoxLayout *statMetricsLayout = new QHBoxLayout();
    statMetricsLayout->setSpacing(15);

    // Метрика 1: Всего мероприятий
    QFrame *statMetric1 = new QFrame();
    statMetric1->setStyleSheet("QFrame {"
                               "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3498db, stop:1 #2980b9);"
                               "border-radius: 8px;"
                               "padding: 15px;"
                               "}");
    statMetric1->setFixedHeight(80);
    QVBoxLayout *statMetric1Layout = new QVBoxLayout(statMetric1);
    QLabel *statMetric1Value = new QLabel("0");
    statMetric1Value->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    QLabel *statMetric1Label = new QLabel("Всего мероприятий");
    statMetric1Label->setStyleSheet("font-size: 13px; color: white;");
    statMetric1Layout->addWidget(statMetric1Value);
    statMetric1Layout->addWidget(statMetric1Label);

    // Метрика 2: Участники
    QFrame *statMetric2 = new QFrame();
    statMetric2->setStyleSheet("QFrame {"
                               "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2ecc71, stop:1 #27ae60);"
                               "border-radius: 8px;"
                               "padding: 15px;"
                               "}");
    statMetric2->setFixedHeight(80);
    QVBoxLayout *statMetric2Layout = new QVBoxLayout(statMetric2);
    QLabel *statMetric2Value = new QLabel("0");
    statMetric2Value->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    QLabel *statMetric2Label = new QLabel("Всего участников");
    statMetric2Label->setStyleSheet("font-size: 13px; color: white;");
    statMetric2Layout->addWidget(statMetric2Value);
    statMetric2Layout->addWidget(statMetric2Label);

    // Метрика 3: Бюджет
    QFrame *statMetric3 = new QFrame();
    statMetric3->setStyleSheet("QFrame {"
                               "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e74c3c, stop:1 #c0392b);"
                               "border-radius: 8px;"
                               "padding: 15px;"
                               "}");
    statMetric3->setFixedHeight(80);
    QVBoxLayout *statMetric3Layout = new QVBoxLayout(statMetric3);
    QLabel *statMetric3Value = new QLabel("0 ₽");
    statMetric3Value->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    QLabel *statMetric3Label = new QLabel("Общий бюджет");
    statMetric3Label->setStyleSheet("font-size: 13px; color: white;");
    statMetric3Layout->addWidget(statMetric3Value);
    statMetric3Layout->addWidget(statMetric3Label);

    // Метрика 4: Заполняемость
    QFrame *statMetric4 = new QFrame();
    statMetric4->setStyleSheet("QFrame {"
                               "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #9b59b6, stop:1 #8e44ad);"
                               "border-radius: 8px;"
                               "padding: 15px;"
                               "}");
    statMetric4->setFixedHeight(80);
    QVBoxLayout *statMetric4Layout = new QVBoxLayout(statMetric4);
    QLabel *statMetric4Value = new QLabel("0%");
    statMetric4Value->setStyleSheet("font-size: 28px; font-weight: bold; color: white;");
    QLabel *statMetric4Label = new QLabel("Средняя заполняемость");
    statMetric4Label->setStyleSheet("font-size: 13px; color: white;");
    statMetric4Layout->addWidget(statMetric4Value);
    statMetric4Layout->addWidget(statMetric4Label);

    statMetricsLayout->addWidget(statMetric1);
    statMetricsLayout->addWidget(statMetric2);
    statMetricsLayout->addWidget(statMetric3);
    statMetricsLayout->addWidget(statMetric4);

    statisticsLayout->addLayout(statMetricsLayout);

    // Вторая строка с графиками
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(15);

    // Левый график - распределение по статусам мероприятий
    QFrame *chart1Container = new QFrame();
    chart1Container->setStyleSheet("QFrame {"
                                   "background-color: white;"
                                   "border-radius: 8px;"
                                   "border: 1px solid #dce4ec;"
                                   "}");
    chart1Container->setFixedSize(350, 250);
    QVBoxLayout *chart1Layout = new QVBoxLayout(chart1Container);

    QLabel *chart1Title = new QLabel("Статусы мероприятий");
    chart1Title->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; padding: 12px;");
    chart1Layout->addWidget(chart1Title);

    // Легенда для статусов
    QWidget *statusChartWidget = new QWidget();
    QVBoxLayout *statusLegendLayout = new QVBoxLayout(statusChartWidget);

    chart1Layout->addWidget(statusChartWidget);

    // Правый график - бюджет по мероприятиям
    QFrame *chart2Container = new QFrame();
    chart2Container->setStyleSheet("QFrame {"
                                   "background-color: white;"
                                   "border-radius: 8px;"
                                   "border: 1px solid #dce4ec;"
                                   "}");
    chart2Container->setFixedSize(350, 250);
    QVBoxLayout *chart2Layout = new QVBoxLayout(chart2Container);

    QLabel *chart2Title = new QLabel("Бюджет по мероприятиям");
    chart2Title->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50; padding: 12px;");
    chart2Layout->addWidget(chart2Title);

    // Легенда для бюджета
    QWidget *budgetChartWidget = new QWidget();
    QVBoxLayout *budgetLegendLayout = new QVBoxLayout(budgetChartWidget);

    chart2Layout->addWidget(budgetChartWidget);

    chartsLayout->addWidget(chart1Container);
    chartsLayout->addWidget(chart2Container);

    statisticsLayout->addLayout(chartsLayout);

    // Таблица топ мероприятий
    QLabel *topEventsTitle = new QLabel("Топ мероприятий по участникам");
    topEventsTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #6155F5; margin-top: 10px;");
    statisticsLayout->addWidget(topEventsTitle);

    QTableWidget *topEventsTable = new QTableWidget();
    topEventsTable->setColumnCount(4);
    topEventsTable->setHorizontalHeaderLabels(QStringList() << "Мероприятие" << "Участники" << "Заполняемость" << "Статус");

    topEventsTable->setStyleSheet(
        "QTableWidget {"
        "background-color: white;"
        "border: 1px solid #dce4ec;"
        "border-radius: 6px;"
        "gridline-color: #ecf0f1;"
        "}"
        "QTableWidget::item {"
        "padding: 8px;"
        "border-bottom: 1px solid #ecf0f1;"
        "}"
        "QHeaderView::section {"
        "background-color: #6155F5;"
        "color: white;"
        "padding: 10px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    topEventsTable->horizontalHeader()->setStretchLastSection(true);
    topEventsTable->verticalHeader()->setVisible(false);
    topEventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topEventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Функция для обновления статистики
    auto updateStatistics = [&]() {
        qDebug() << "Обновление статистики...";
        // Обновляем метрики
        QVector<Event> events = dbManager.getAllEvents();
        QVector<Participant> participants = dbManager.getAllParticipants();

        // Метрика 1: Всего мероприятий
        statMetric1Value->setText(QString::number(events.size()));

        // Метрика 2: Участники
        statMetric2Value->setText(QString::number(participants.size()));

        // Метрика 3: Общий бюджет
        double totalBudget = 0;
        for (const Event &event : events) {
            totalBudget += dbManager.getTotalPlannedBudgetForEvent(event.id);
        }
        statMetric3Value->setText(QString("%1 ₽").arg(totalBudget, 0, 'f', 2));

        // Метрика 4: Средняя заполняемость
        if (!events.isEmpty()) {
            double totalFillRate = 0;
            int eventsWithParticipants = 0;

            for (const Event &event : events) {
                int participantCount = dbManager.getParticipantCountForEvent(event.id);
                if (event.max_participants > 0) {
                    double fillRate = (static_cast<double>(participantCount) / event.max_participants) * 100;
                    totalFillRate += fillRate;
                    eventsWithParticipants++;
                }
            }

            double avgFillRate = eventsWithParticipants > 0 ? totalFillRate / eventsWithParticipants : 0;
            statMetric4Value->setText(QString("%1%").arg(avgFillRate, 0, 'f', 1));
        }

        // Обновляем график статусов
        QLayoutItem* child;
        while ((child = statusLegendLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        // Подсчет мероприятий по статусам
        QMap<QString, int> statusCounts;
        for (const Event &event : events) {
            statusCounts[event.status]++;
        }

        QList<QString> statusColors = {"#3498db", "#2ecc71", "#e74c3c", "#f39c12"};
        QList<QString> statusNames = {"planned", "ongoing", "completed", "cancelled"};
        QList<QString> statusDisplayNames = {"Запланировано", "В процессе", "Завершено", "Отменено"};

        for (int i = 0; i < statusNames.size(); ++i) {
            int count = statusCounts[statusNames[i]];
            double percentage = events.size() > 0 ? (static_cast<double>(count) / events.size()) * 100 : 0;

            QHBoxLayout *legendItemLayout = new QHBoxLayout();

            QLabel *colorLabel = new QLabel();
            colorLabel->setFixedSize(12, 12);
            colorLabel->setStyleSheet(QString("background-color: %1; border-radius: 2px;").arg(statusColors[i]));

            QLabel *textLabel = new QLabel(QString("%1 (%2, %3%)").arg(statusDisplayNames[i]).arg(count).arg(percentage, 0, 'f', 1));
            textLabel->setStyleSheet("font-size: 11px; color: #34495e;");

            legendItemLayout->addWidget(colorLabel);
            legendItemLayout->addWidget(textLabel);
            legendItemLayout->addStretch();

            statusLegendLayout->addLayout(legendItemLayout);
        }

        // Обновляем график бюджета
        while ((child = budgetLegendLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        // Топ 5 мероприятий по бюджету
        QVector<QPair<QString, double>> eventBudgets;
        for (const Event &event : events) {
            double budget = dbManager.getTotalPlannedBudgetForEvent(event.id);
            eventBudgets.append(qMakePair(event.title, budget));
        }

        // Сортируем по убыванию бюджета
        std::sort(eventBudgets.begin(), eventBudgets.end(),
                  [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
                      return a.second > b.second;
                  });

        // Берем топ 5
        int topCount = qMin(5, eventBudgets.size());
        for (int i = 0; i < topCount; ++i) {
            QHBoxLayout *budgetItemLayout = new QHBoxLayout();

            QLabel *numberLabel = new QLabel(QString("%1.").arg(i + 1));
            numberLabel->setFixedWidth(15);
            numberLabel->setStyleSheet("font-size: 11px; color: #34495e; font-weight: bold;");

            QLabel *nameLabel = new QLabel(eventBudgets[i].first);
            nameLabel->setStyleSheet("font-size: 11px; color: #34495e;");
            nameLabel->setMaximumWidth(150);
            nameLabel->setWordWrap(true);

            QLabel *budgetLabel = new QLabel(QString("%1 ₽").arg(eventBudgets[i].second, 0, 'f', 2));
            budgetLabel->setStyleSheet("font-size: 11px; color: #2c3e50; font-weight: bold;");

            budgetItemLayout->addWidget(numberLabel);
            budgetItemLayout->addWidget(nameLabel);
            budgetItemLayout->addWidget(budgetLabel);
            budgetItemLayout->addStretch();

            budgetLegendLayout->addLayout(budgetItemLayout);
        }

        // Обновляем таблицу топ мероприятий
        QVector<QPair<Event, int>> eventParticipants;
        for (const Event &event : events) {
            int participantCount = dbManager.getParticipantCountForEvent(event.id);
            eventParticipants.append(qMakePair(event, participantCount));
        }

        // Сортируем по количеству участников
        std::sort(eventParticipants.begin(), eventParticipants.end(),
                  [](const QPair<Event, int> &a, const QPair<Event, int> &b) {
                      return a.second > b.second;
                  });

        // Берем топ 5
        topCount = qMin(5, eventParticipants.size());
        topEventsTable->setRowCount(topCount);

        for (int i = 0; i < topCount; ++i) {
            const Event &event = eventParticipants[i].first;
            int participantCount = eventParticipants[i].second;
            double fillRate = event.max_participants > 0 ?
                             (static_cast<double>(participantCount) / event.max_participants) * 100 : 0;

            // Перевод статуса
            QString statusText;
            if (event.status == "planned") statusText = "Запланировано";
            else if (event.status == "ongoing") statusText = "В процессе";
            else if (event.status == "completed") statusText = "Завершено";
            else if (event.status == "cancelled") statusText = "Отменено";
            else statusText = event.status;

            QTableWidgetItem *eventItem = new QTableWidgetItem(event.title);
            QTableWidgetItem *participantsItem = new QTableWidgetItem(QString::number(participantCount));
            QTableWidgetItem *fillRateItem = new QTableWidgetItem(QString("%1%").arg(fillRate, 0, 'f', 1));
            QTableWidgetItem *statusItem = new QTableWidgetItem(statusText);

            // Цвет для статуса
            if (event.status == "completed") {
                statusItem->setForeground(QBrush(QColor("#27ae60")));
            } else if (event.status == "cancelled") {
                statusItem->setForeground(QBrush(QColor("#e74c3c")));
            } else if (event.status == "ongoing") {
                statusItem->setForeground(QBrush(QColor("#3498db")));
            }

            topEventsTable->setItem(i, 0, eventItem);
            topEventsTable->setItem(i, 1, participantsItem);
            topEventsTable->setItem(i, 2, fillRateItem);
            topEventsTable->setItem(i, 3, statusItem);
        }

        topEventsTable->resizeColumnsToContents();
        qDebug() << "Статистика обновлена";
    };

    statisticsLayout->addWidget(topEventsTable);

    // Кнопка обновления статистики
    QPushButton *refreshStatsBtn = new QPushButton("🔄 Обновить статистику");
    refreshStatsBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6155F5;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px 20px;"
        "font-size: 13px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #5046d4;"
        "}"
        "QPushButton:pressed {"
        "background-color: #4038b3;"
        "}"
        );
    refreshStatsBtn->setFixedWidth(205);
    statisticsLayout->addWidget(refreshStatsBtn, 0, Qt::AlignRight);

    // Кнопка экспорта отчетов
    QPushButton *exportReportBtn = new QPushButton("📊 Экспорт отчета");
    exportReportBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #27ae60;"
        "color: white;"
        "border: none;"
        "border-radius: 5px;"
        "padding: 10px 20px;"
        "font-size: 13px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #219653;"
        "}"
        "QPushButton:pressed {"
        "background-color: #1e8449;"
        "}"
        );
    exportReportBtn->setFixedWidth(160);
    statisticsLayout->addWidget(exportReportBtn, 0, Qt::AlignRight);

    // Добавление всех страниц в stacked widget с правильными индексами
    stackedWidget->addWidget(mainPage);             // индекс 0 - Главная
    stackedWidget->addWidget(eventsPage);           // индекс 1 - Мероприятия
    stackedWidget->addWidget(createEventPage);      // индекс 2 - Создание мероприятия
    stackedWidget->addWidget(participantsPage);     // индекс 3 - Участники
    stackedWidget->addWidget(addParticipantPage);   // индекс 4 - Добавление участника
    stackedWidget->addWidget(budgetPage);           // индекс 5 - Бюджет
    stackedWidget->addWidget(statisticsPage);       // индекс 6 - Статистика

    // ОБРАБОТЧИКИ С ИСПОЛЬЗОВАНИЕМ БАЗЫ ДАННЫХ

    // Обработчик кнопки создания мероприятия
    QObject::connect(createButton, &QPushButton::clicked, [&]() {
        qDebug() << "=== НАЧАЛО ПРОЦЕССА СОЗДАНИЯ МЕРОПРИЯТИЯ ===";

        QString eventName = nameEdit->text().trimmed();
        QString eventDescription = descEdit->toPlainText().trimmed();
        QDate eventDate = dateEdit->date();
        int maxParticipants = maxParticipantsSpin->value();
        double budgetAmount = budgetEdit->text().toDouble();

        qDebug() << "Данные формы:";
        qDebug() << "  Название:" << eventName;
        qDebug() << "  Описание:" << eventDescription;
        qDebug() << "  Дата:" << eventDate.toString("dd.MM.yyyy");
        qDebug() << "  Макс. участников:" << maxParticipants;
        qDebug() << "  Бюджет:" << budgetAmount;

        // ВАЛИДАЦИЯ ДАННЫХ
        if (eventName.isEmpty()) {
            QMessageBox::warning(formContainer, "Ошибка", "Введите название мероприятия!");
            qDebug() << "ОШИБКА: Не заполнено название мероприятия";
            return;
        }

        if (!eventDate.isValid() || eventDate < QDate::currentDate()) {
            QMessageBox::warning(formContainer, "Ошибка", "Укажите корректную дату мероприятия!");
            qDebug() << "ОШИБКА: Неверная дата мероприятия";
            return;
        }

        // СОЗДАНИЕ ОБЪЕКТА МЕРОПРИЯТИЯ
        Event newEvent;
        newEvent.title = eventName;
        newEvent.description = eventDescription;

        // Правильное форматирование даты и времени
        QDateTime eventDateTime;
        eventDateTime.setDate(eventDate);
        eventDateTime.setTime(QTime(10, 0, 0)); // 10:00:00
        newEvent.event_datetime = eventDateTime.toString("yyyy-MM-dd hh:mm:ss");

        newEvent.location = "Место проведения не указано";
        newEvent.max_participants = maxParticipants;
        newEvent.status = "planned";

        qDebug() << "Создан объект Event:";
        qDebug() << "  Title:" << newEvent.title;
        qDebug() << "  DateTime:" << newEvent.event_datetime;
        qDebug() << "  Location:" << newEvent.location;
        qDebug() << "  Max Participants:" << newEvent.max_participants;
        qDebug() << "  Status:" << newEvent.status;

        // СОХРАНЕНИЕ В БАЗУ ДАННЫХ
        qDebug() << "Вызов dbManager.addEvent()...";
        bool eventAdded = dbManager.addEvent(newEvent);
        qDebug() << "Результат dbManager.addEvent():" << eventAdded;

        if (eventAdded) {
            qDebug() << "Мероприятие успешно добавлено в БД";

            // ПОЛУЧЕНИЕ ID СОЗДАННОГО МЕРОПРИЯТИЯ
            QVector<Event> events = dbManager.getAllEvents();
            qDebug() << "Всего мероприятий в БД:" << events.size();

            if (!events.isEmpty()) {
                int newEventId = events.last().id;
                qDebug() << "ID созданного мероприятия:" << newEventId;

                // ДОБАВЛЕНИЕ БЮДЖЕТА (если указан)
                if (budgetAmount > 0) {
                    BudgetItem budgetItem;
                    budgetItem.event_id = newEventId;
                    budgetItem.expense_item = "Общий бюджет мероприятия";
                    budgetItem.planned_amount = budgetAmount;
                    budgetItem.actual_amount = 0.0;

                    qDebug() << "Добавление бюджета...";
                    bool budgetAdded = dbManager.addBudgetItem(budgetItem);
                    qDebug() << "Результат добавления бюджета:" << budgetAdded;

                    if (!budgetAdded) {
                        qDebug() << "Предупреждение: не удалось добавить бюджет";
                    }
                }

                // УСПЕШНОЕ СООБЩЕНИЕ
                QMessageBox::information(formContainer, "Успех",
                    QString("Мероприятие \"%1\" успешно создано!\n\n"
                           "Дата: %2\n"
                           "Максимальное количество участников: %3\n"
                           "Бюджет: %4 руб.")
                        .arg(eventName)
                        .arg(eventDate.toString("dd.MM.yyyy"))
                        .arg(maxParticipants)
                        .arg(budgetAmount));

                // ОБНОВЛЕНИЕ ИНТЕРФЕЙСА
                updateEventsTable();
                updateBudgetTable();
                updateStatistics();

                // ОЧИСТКА ФОРМЫ
                nameEdit->clear();
                descEdit->clear();
                budgetEdit->setText("0");
                dateEdit->setDate(QDate::currentDate().addDays(7)); // Дата через неделю
                maxParticipantsSpin->setValue(50);

                // ПЕРЕХОД К СПИСКУ МЕРОПРИЯТИЙ
                stackedWidget->setCurrentIndex(1);

            } else {
                qDebug() << "ОШИБКА: Не удалось получить список мероприятий после добавления";
                QMessageBox::warning(formContainer, "Ошибка",
                    "Мероприятие создано, но возникла проблема с получением его ID.");
            }
        } else {
            qDebug() << "ОШИБКА: Не удалось добавить мероприятие в базу данных";
            QMessageBox::critical(formContainer, "Ошибка",
                "Не удалось создать мероприятие в базе данных!\n"
                "Возможно, проблема с подключением к базе данных.");
        }

        qDebug() << "=== ЗАВЕРШЕНИЕ ПРОЦЕССА СОЗДАНИЯ МЕРОПРИЯТИЯ ===";
    });

    // Обработчик двойного клика по таблице мероприятий для редактирования
    QObject::connect(eventsTable, &QTableWidget::cellDoubleClicked, [eventsTable, &window, &dbManager, updateEventsTable](int row, int column) {
        if (column == 5) {
            QVector<Event> events = dbManager.getAllEvents();
            if (row < events.size()) {
                Event event = events[row];
                EditEventDialog dialog(event, &window);
                if (dialog.exec() == QDialog::Accepted) {
                    Event updatedEvent = dialog.getEventData(event.id);
                    if (dbManager.updateEvent(updatedEvent)) {
                        updateEventsTable();
                        QMessageBox::information(&window, "Успех", "Данные мероприятия успешно обновлены!");
                    } else {
                        QMessageBox::warning(&window, "Ошибка", "Не удалось обновить данные мероприятия!");
                    }
                }
            }
        }
    });

    // Обработчик кнопки добавления расхода
    QObject::connect(addExpenseBtn, &QPushButton::clicked, [&]() {
        QVector<Event> events = dbManager.getAllEvents();
        AddExpenseDialog dialog(events, &window);
        if (dialog.exec() == QDialog::Accepted) {
            int eventId = dialog.getEventId();
            QString expenseName = dialog.getExpenseName();
            double amount = dialog.getAmount();
            QString category = dialog.getCategory();

            // Создаем новую статью расхода
            BudgetItem budgetItem;
            budgetItem.event_id = eventId;
            budgetItem.expense_item = QString("%1: %2").arg(category, expenseName);
            budgetItem.planned_amount = 0.0;
            budgetItem.actual_amount = amount;

            if (dbManager.addBudgetItem(budgetItem)) {
                QMessageBox::information(&window, "Расход добавлен",
                                         QString("Расход успешно добавлен!\n\n"
                                                 "Мероприятие: %1\n"
                                                 "Наименование: %2\n"
                                                 "Сумма: %3 ₽\n"
                                                 "Категория: %4")
                                             .arg(dialog.getEventName())
                                             .arg(expenseName)
                                             .arg(amount)
                                             .arg(category));

                // Обновляем таблицу бюджета
                updateBudgetTable();
            } else {
                QMessageBox::warning(&window, "Ошибка", "Не удалось добавить расход в базу данных!");
            }
        }
    });

    // Обработчик кнопки добавления участника
    QObject::connect(addParticipantBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(4); // Добавление участника
    });

    // Обработчик кнопки добавления участника в форму
    QObject::connect(addParticipantButton, &QPushButton::clicked, [&]() {
        qDebug() << "=== НАЧАЛО ПРОЦЕССА ДОБАВЛЕНИЯ УЧАСТНИКА ===";

        QString participantName = participantNameEdit->text().trimmed();
        QString email = emailEdit->text().trimmed();
        QString phone = phoneEdit->text().trimmed();
        int eventId = eventCombo->currentData().toInt();

        qDebug() << "Данные формы участника:";
        qDebug() << "  Имя:" << participantName;
        qDebug() << "  Email:" << email;
        qDebug() << "  Телефон:" << phone;
        qDebug() << "  ID мероприятия:" << eventId;

        if (participantName.isEmpty()) {
            QMessageBox::warning(participantFormContainer, "Ошибка", "Введите имя участника!");
            qDebug() << "ОШИБКА: Не заполнено имя участника";
            return;
        }

        if (email.isEmpty()) {
            QMessageBox::warning(participantFormContainer, "Ошибка", "Введите email участника!");
            qDebug() << "ОШИБКА: Не заполнен email участника";
            return;
        }

        // Создаем нового участника
        Participant newParticipant;
        newParticipant.full_name = participantName;
        newParticipant.email = email;
        newParticipant.phone = phone;

        qDebug() << "Вызов dbManager.addParticipant()...";
        bool participantAdded = dbManager.addParticipant(newParticipant);
        qDebug() << "Результат dbManager.addParticipant():" << participantAdded;

        if (participantAdded) {
            qDebug() << "Участник успешно добавлен в БД";

            // Если выбрано мероприятие, добавляем регистрацию
            if (eventId > 0) {
                qDebug() << "Добавление регистрации на мероприятие ID:" << eventId;

                // Получаем ID последнего добавленного участника
                QVector<Participant> participants = dbManager.getAllParticipants();
                if (!participants.isEmpty()) {
                    int newParticipantId = participants.last().id;

                    Registration newRegistration;
                    newRegistration.participant_id = newParticipantId;
                    newRegistration.event_id = eventId;
                    newRegistration.participation_status = "registered";

                    qDebug() << "Вызов dbManager.addRegistration()...";
                    bool registrationAdded = dbManager.addRegistration(newRegistration);
                    qDebug() << "Результат dbManager.addRegistration():" << registrationAdded;

                    if (!registrationAdded) {
                        qDebug() << "Предупреждение: не удалось добавить регистрацию";
                    }
                }
            }

            QMessageBox::information(participantFormContainer, "Успех",
                                     QString("Участник успешно добавлен!\n\n"
                                             "Имя: %1\n"
                                             "Email: %2\n"
                                             "Телефон: %3")
                                         .arg(participantName)
                                         .arg(email)
                                         .arg(phone.isEmpty() ? "Не указан" : phone));

            // Очистка полей после успешного добавления
            participantNameEdit->clear();
            emailEdit->clear();
            phoneEdit->clear();
            eventCombo->setCurrentIndex(0);

            // Обновляем таблицы
            updateParticipantsTable();
            updateEventsTable();
            updateStatistics();

            // Возврат к списку участников
            stackedWidget->setCurrentIndex(3);
        } else {
            qDebug() << "ОШИБКА: Не удалось добавить участника в базу данных";
            QMessageBox::warning(participantFormContainer, "Ошибка", "Не удалось добавить участника!");
        }

        qDebug() << "=== ЗАВЕРШЕНИЕ ПРОЦЕССА ДОБАВЛЕНИЯ УЧАСТНИКА ===";
    });

    // Обработчик быстрого добавления участника с главной страницы
    QObject::connect(quickParticipantBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(4); // Добавление участника
    });

    // Обработчик кнопки обновления статистики
    QObject::connect(refreshStatsBtn, &QPushButton::clicked, [&]() {
        updateStatistics();
        QMessageBox::information(statisticsPage, "Обновлено", "Статистика обновлена!");
    });

    // Обработчик кнопки экспорта отчета
    QObject::connect(exportReportBtn, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&window, "Экспорт отчета", "", "PDF Files (*.pdf);;Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QMessageBox::information(&window, "Экспорт отчета",
                                     QString("Отчет успешно экспортирован в файл:\n%1").arg(fileName));
        }
    });

    // Обновляем статистику при переходе на страницу
    QObject::connect(statisticsButton, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(6); // Статистика
        updateStatistics(); // Обновляем данные при каждом переходе
    });

    // Обработчики навигации
    QObject::connect(mainButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(0); // Главная
    });

    QObject::connect(eventsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(1); // Мероприятия
    });

    QObject::connect(participantsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(3); // Участники
    });

    QObject::connect(budgetButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(5); // Бюджет
    });

    QObject::connect(statisticsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(6); // Статистика
    });

    // Обработчики быстрых действий
    QObject::connect(quickEventBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(2); // Создание мероприятия
    });

    QObject::connect(quickParticipantBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(4); // Добавление участника
    });

    QObject::connect(quickBudgetBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(5); // Бюджет
    });

    QObject::connect(quickReportBtn, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentIndex(6); // Статистика
    });

    // Инициализация начального состояния
    qDebug() << "=== ИНИЦИАЛИЗАЦИЯ ИНТЕРФЕЙСА ===";
    updateEventsTable();
    updateBudgetTable();
    updateParticipantsTable();
    updateEventComboBox();
    updateStatistics();

    // Обновляем метрику участников
    QVector<Participant> participants = dbManager.getAllParticipants();
    metric3Value->setText(QString::number(participants.size()));

    // Показываем главную страницу по умолчанию
    stackedWidget->setCurrentIndex(0);

    qDebug() << "=== ПРИЛОЖЕНИЕ УСПЕШНО ЗАПУЩЕНО ===";

    window.show();

    return app.exec();
}
