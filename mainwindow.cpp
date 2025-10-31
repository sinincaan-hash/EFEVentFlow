#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Инициализация базы данных
    if (!initDatabase()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось инициализировать базу данных!");
        return;
    }

    // Создание таблиц, если они не существуют
    if (!createTables()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать таблицы базы данных!");
        return;
    }
}

MainWindow::~MainWindow()
{
    // Закрываем соединение с базой данных
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

bool MainWindow::initDatabase()
{
    // Создаем соединение с SQLite базой данных
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("event_management.db");

    if (!db.open()) {
        qDebug() << "Ошибка открытия базы данных:" << db.lastError().text();
        return false;
    }

    qDebug() << "База данных успешно открыта";
    return true;
}

bool MainWindow::createTables()
{
    QSqlQuery query;

    // Таблица Events
    QString createEventsTable =
        "CREATE TABLE IF NOT EXISTS Events ("
        "event_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "event_datetime DATETIME NOT NULL,"
        "location TEXT NOT NULL,"
        "max_participants INTEGER,"
        "status TEXT DEFAULT 'planned' CHECK(status IN ('planned', 'ongoing', 'completed', 'cancelled')),"
        "created_date DATETIME DEFAULT CURRENT_TIMESTAMP)";

    if (!query.exec(createEventsTable)) {
        qDebug() << "Ошибка создания таблицы Events:" << query.lastError().text();
        return false;
    }

    // Таблица Participants
    QString createParticipantsTable =
        "CREATE TABLE IF NOT EXISTS Participants ("
        "participant_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "full_name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "phone TEXT,"
        "reg_date DATETIME DEFAULT CURRENT_TIMESTAMP)";

    if (!query.exec(createParticipantsTable)) {
        qDebug() << "Ошибка создания таблицы Participants:" << query.lastError().text();
        return false;
    }

    // Таблица Registrations
    QString createRegistrationsTable =
        "CREATE TABLE IF NOT EXISTS Registrations ("
        "registration_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "participant_id INTEGER NOT NULL,"
        "event_id INTEGER NOT NULL,"
        "participation_status TEXT DEFAULT 'registered' CHECK(participation_status IN ('registered', 'confirmed', 'attended', 'cancelled')),"
        "reg_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (participant_id) REFERENCES Participants(participant_id) ON DELETE CASCADE,"
        "FOREIGN KEY (event_id) REFERENCES Events(event_id) ON DELETE CASCADE,"
        "UNIQUE(participant_id, event_id))";

    if (!query.exec(createRegistrationsTable)) {
        qDebug() << "Ошибка создания таблицы Registrations:" << query.lastError().text();
        return false;
    }

    // Таблица Budget
    QString createBudgetTable =
        "CREATE TABLE IF NOT EXISTS Budget ("
        "budget_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "event_id INTEGER NOT NULL,"
        "expense_item TEXT NOT NULL,"
        "planned_amount DECIMAL(10,2) NOT NULL,"
        "actual_amount DECIMAL(10,2),"
        "date_created DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (event_id) REFERENCES Events(event_id) ON DELETE CASCADE)";

    if (!query.exec(createBudgetTable)) {
        qDebug() << "Ошибка создания таблицы Budget:" << query.lastError().text();
        return false;
    }

    // Создание индексов
    QStringList indexes = {
        "CREATE INDEX IF NOT EXISTS idx_events_datetime ON Events(event_datetime)",
        "CREATE INDEX IF NOT EXISTS idx_events_status ON Events(status)",
        "CREATE INDEX IF NOT EXISTS idx_registrations_event ON Registrations(event_id)",
        "CREATE INDEX IF NOT EXISTS idx_registrations_participant ON Registrations(participant_id)",
        "CREATE INDEX IF NOT EXISTS idx_registrations_status ON Registrations(participation_status)",
        "CREATE INDEX IF NOT EXISTS idx_budget_event ON Budget(event_id)"
    };

    for (const QString &indexSql : indexes) {
        if (!query.exec(indexSql)) {
            qDebug() << "Ошибка создания индекса:" << query.lastError().text();
        }
    }

    qDebug() << "Все таблицы успешно созданы";
    return true;
}

// Методы для работы с событиями
bool MainWindow::addEvent(const Event &event)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Events (title, description, event_datetime, location, max_participants, status) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(event.title);
    query.addBindValue(event.description);
    query.addBindValue(event.event_datetime);
    query.addBindValue(event.location);
    query.addBindValue(event.max_participants);
    query.addBindValue(event.status);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления события:" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<Event> MainWindow::getAllEvents()
{
    QVector<Event> events;
    QSqlQuery query("SELECT event_id, title, description, event_datetime, location, max_participants, status, created_date FROM Events ORDER BY event_datetime");

    while (query.next()) {
        Event event;
        event.id = query.value(0).toInt();
        event.title = query.value(1).toString();
        event.description = query.value(2).toString();
        event.event_datetime = query.value(3).toString();
        event.location = query.value(4).toString();
        event.max_participants = query.value(5).toInt();
        event.status = query.value(6).toString();
        event.created_date = query.value(7).toString();

        events.append(event);
    }

    return events;
}

bool MainWindow::updateEvent(const Event &event)
{
    QSqlQuery query;
    query.prepare("UPDATE Events SET title = ?, description = ?, event_datetime = ?, location = ?, max_participants = ?, status = ? WHERE event_id = ?");
    query.addBindValue(event.title);
    query.addBindValue(event.description);
    query.addBindValue(event.event_datetime);
    query.addBindValue(event.location);
    query.addBindValue(event.max_participants);
    query.addBindValue(event.status);
    query.addBindValue(event.id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления события:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool MainWindow::deleteEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Events WHERE event_id = ?");
    query.addBindValue(event_id);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления события:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Методы для работы с участниками
bool MainWindow::addParticipant(const Participant &participant)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Participants (full_name, email, phone) VALUES (?, ?, ?)");
    query.addBindValue(participant.full_name);
    query.addBindValue(participant.email);
    query.addBindValue(participant.phone);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления участника:" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<Participant> MainWindow::getAllParticipants()
{
    QVector<Participant> participants;
    QSqlQuery query("SELECT participant_id, full_name, email, phone, reg_date FROM Participants ORDER BY full_name");

    while (query.next()) {
        Participant participant;
        participant.id = query.value(0).toInt();
        participant.full_name = query.value(1).toString();
        participant.email = query.value(2).toString();
        participant.phone = query.value(3).toString();
        participant.reg_date = query.value(4).toString();

        participants.append(participant);
    }

    return participants;
}

bool MainWindow::updateParticipant(const Participant &participant)
{
    QSqlQuery query;
    query.prepare("UPDATE Participants SET full_name = ?, email = ?, phone = ? WHERE participant_id = ?");
    query.addBindValue(participant.full_name);
    query.addBindValue(participant.email);
    query.addBindValue(participant.phone);
    query.addBindValue(participant.id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления участника:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Методы для работы с регистрациями
bool MainWindow::addRegistration(const Registration &registration)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Registrations (participant_id, event_id, participation_status) VALUES (?, ?, ?)");
    query.addBindValue(registration.participant_id);
    query.addBindValue(registration.event_id);
    query.addBindValue(registration.participation_status);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления регистрации:" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<Registration> MainWindow::getRegistrationsForEvent(int event_id)
{
    QVector<Registration> registrations;
    QSqlQuery query;
    query.prepare("SELECT r.registration_id, r.participant_id, r.event_id, r.participation_status, r.reg_date, "
                  "p.full_name, p.email FROM Registrations r "
                  "JOIN Participants p ON r.participant_id = p.participant_id "
                  "WHERE r.event_id = ? ORDER BY r.reg_date");
    query.addBindValue(event_id);

    if (query.exec()) {
        while (query.next()) {
            Registration reg;
            reg.registration_id = query.value(0).toInt();
            reg.participant_id = query.value(1).toInt();
            reg.event_id = query.value(2).toInt();
            reg.participation_status = query.value(3).toString();
            reg.reg_date = query.value(4).toString();

            registrations.append(reg);
        }
    }

    return registrations;
}

int MainWindow::getParticipantCountForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Registrations WHERE event_id = ? AND participation_status != 'cancelled'");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

// Методы для работы с бюджетом
bool MainWindow::addBudgetItem(const BudgetItem &budgetItem)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Budget (event_id, expense_item, planned_amount, actual_amount) VALUES (?, ?, ?, ?)");
    query.addBindValue(budgetItem.event_id);
    query.addBindValue(budgetItem.expense_item);
    query.addBindValue(budgetItem.planned_amount);
    query.addBindValue(budgetItem.actual_amount);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления статьи бюджета:" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<BudgetItem> MainWindow::getBudgetForEvent(int event_id)
{
    QVector<BudgetItem> budgetItems;
    QSqlQuery query;
    query.prepare("SELECT budget_id, event_id, expense_item, planned_amount, actual_amount, date_created FROM Budget WHERE event_id = ? ORDER BY date_created");
    query.addBindValue(event_id);

    if (query.exec()) {
        while (query.next()) {
            BudgetItem item;
            item.budget_id = query.value(0).toInt();
            item.event_id = query.value(1).toInt();
            item.expense_item = query.value(2).toString();
            item.planned_amount = query.value(3).toDouble();
            item.actual_amount = query.value(4).toDouble();
            item.date_created = query.value(5).toString();

            budgetItems.append(item);
        }
    }

    return budgetItems;
}

double MainWindow::getTotalPlannedBudgetForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(planned_amount) FROM Budget WHERE event_id = ?");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

double MainWindow::getTotalActualBudgetForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(actual_amount) FROM Budget WHERE event_id = ?");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}
