#include "database_manager.h"

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initDatabase()
{
    qDebug() << "=== DatabaseManager::initDatabase ===";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("event_management.db");

    qDebug() << "Пытаемся открыть базу данных:" << db.databaseName();
    qDebug() << "Драйвер доступен?" << QSqlDatabase::isDriverAvailable("QSQLITE");

    if (!db.open()) {
        qDebug() << "ОШИБКА открытия базы данных:" << db.lastError().text();
        qDebug() << "Тип ошибки:" << db.lastError().type();
        qDebug() << "Код ошибки:" << db.lastError().number();
        return false;
    }

    qDebug() << "База данных успешно открыта";
    qDebug() << "Подключение к БД:" << db.connectionName();

    return true;
}

bool DatabaseManager::createTables()
{
    qDebug() << "=== DatabaseManager::createTables ===";

    // Проверяем подключение к БД
    if (!db.isOpen()) {
        qDebug() << "ОШИБКА: База данных не открыта!";
        return false;
    }

    // Получаем список существующих таблиц
    QStringList existingTables = db.tables();
    qDebug() << "Существующие таблицы в БД:" << existingTables;

    QSqlQuery query;

    // Создаем таблицы если их нет
    QStringList tableQueries = {
        // Events table
        "CREATE TABLE IF NOT EXISTS Events ("
        "event_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "event_datetime DATETIME NOT NULL,"
        "location TEXT NOT NULL,"
        "max_participants INTEGER,"
        "status TEXT DEFAULT 'planned' CHECK(status IN ('planned', 'ongoing', 'completed', 'cancelled')),"
        "created_date DATETIME DEFAULT CURRENT_TIMESTAMP)",

        // Participants table
        "CREATE TABLE IF NOT EXISTS Participants ("
        "participant_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "full_name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "phone TEXT,"
        "reg_date DATETIME DEFAULT CURRENT_TIMESTAMP)",

        // Registrations table
        "CREATE TABLE IF NOT EXISTS Registrations ("
        "registration_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "participant_id INTEGER NOT NULL,"
        "event_id INTEGER NOT NULL,"
        "participation_status TEXT DEFAULT 'registered' CHECK(participation_status IN ('registered', 'confirmed', 'attended', 'cancelled')),"
        "reg_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (participant_id) REFERENCES Participants(participant_id) ON DELETE CASCADE,"
        "FOREIGN KEY (event_id) REFERENCES Events(event_id) ON DELETE CASCADE,"
        "UNIQUE(participant_id, event_id))",

        // Budget table
        "CREATE TABLE IF NOT EXISTS Budget ("
        "budget_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "event_id INTEGER NOT NULL,"
        "expense_item TEXT NOT NULL,"
        "planned_amount DECIMAL(10,2) NOT NULL,"
        "actual_amount DECIMAL(10,2),"
        "date_created DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (event_id) REFERENCES Events(event_id) ON DELETE CASCADE)"
    };

    for (int i = 0; i < tableQueries.size(); ++i) {
        qDebug() << "Создание таблицы" << (i+1) << "...";
        if (!query.exec(tableQueries[i])) {
            qDebug() << "ОШИБКА создания таблицы:" << query.lastError().text();
            return false;
        }
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

    // Вставка тестовых данных, если таблицы пустые
    query.exec("SELECT COUNT(*) FROM Events");
    if (query.next() && query.value(0).toInt() == 0) {
        qDebug() << "Добавление тестовых данных...";

        QStringList testEvents = {
            "INSERT INTO Events (title, description, event_datetime, location, max_participants, status) VALUES "
            "('UX/UI Meetup 2024', 'Ежемесячная встреча разработчиков UX/UI', '2024-06-15 19:00:00', 'Москва, Коворкинг Центр', 100, 'planned')",
            "INSERT INTO Events (title, description, event_datetime, location, max_participants, status) VALUES "
            "('Frontend Conf 2024', 'Годовая конференция фронтенд разработчиков', '2024-07-20 10:00:00', 'Санкт-Петербург, Экспофорум', 500, 'planned')",
            "INSERT INTO Events (title, description, event_datetime, location, max_participants, status) VALUES "
            "('Tech Startup Pitch', 'Питч-сессия для IT стартапов', '2024-05-30 14:00:00', 'Онлайн', 200, 'planned')"
        };

        for (const QString &sql : testEvents) {
            if (!query.exec(sql)) {
                qDebug() << "Ошибка добавления тестового мероприятия:" << query.lastError().text();
            }
        }

        // Тестовые участники
        QStringList testParticipants = {
            "INSERT INTO Participants (full_name, email, phone) VALUES "
            "('Иван Петров', 'ivan.petrov@email.com', '+7-999-123-45-67')",
            "INSERT INTO Participants (full_name, email, phone) VALUES "
            "('Мария Сидорова', 'maria.sidorova@email.com', '+7-999-765-43-21')",
            "INSERT INTO Participants (full_name, email, phone) VALUES "
            "('Алексей Козлов', 'alex.kozlov@email.com', '+7-912-345-67-89')"
        };

        for (const QString &sql : testParticipants) {
            if (!query.exec(sql)) {
                qDebug() << "Ошибка добавления тестового участника:" << query.lastError().text();
            }
        }

        // Тестовый бюджет
        if (!query.exec("INSERT INTO Budget (event_id, expense_item, planned_amount, actual_amount) VALUES (1, 'Аренда помещения', 50000.00, 45000.00)")) {
            qDebug() << "Ошибка добавления тестового бюджета:" << query.lastError().text();
        }
        if (!query.exec("INSERT INTO Budget (event_id, expense_item, planned_amount, actual_amount) VALUES (1, 'Кейтеринг', 25000.00, 23000.00)")) {
            qDebug() << "Ошибка добавления тестового бюджета:" << query.lastError().text();
        }
        if (!query.exec("INSERT INTO Budget (event_id, expense_item, planned_amount, actual_amount) VALUES (2, 'Онлайн платформа', 5000.00, 5000.00)")) {
            qDebug() << "Ошибка добавления тестового бюджета:" << query.lastError().text();
        }

        qDebug() << "Тестовые данные добавлены";
    }

    // Проверяем что таблицы создались
    QStringList finalTables = db.tables();
    qDebug() << "Таблицы после создания:" << finalTables;

    qDebug() << "Все таблицы успешно созданы/проверены";
    return true;
}

// Реализации методов работы с событиями
bool DatabaseManager::addEvent(const Event &event)
{
    qDebug() << "=== DatabaseManager::addEvent ===";
    qDebug() << "Полученные данные:";
    qDebug() << "  ID:" << event.id;
    qDebug() << "  Title:" << event.title;
    qDebug() << "  Description:" << event.description;
    qDebug() << "  DateTime:" << event.event_datetime;
    qDebug() << "  Location:" << event.location;
    qDebug() << "  Max Participants:" << event.max_participants;
    qDebug() << "  Status:" << event.status;

    QSqlQuery query;
    query.prepare("INSERT INTO Events (title, description, event_datetime, location, max_participants, status) "
                  "VALUES (?, ?, ?, ?, ?, ?)");

    query.addBindValue(event.title);
    query.addBindValue(event.description);
    query.addBindValue(event.event_datetime);
    query.addBindValue(event.location);
    query.addBindValue(event.max_participants);
    query.addBindValue(event.status);

    qDebug() << "Выполнение SQL запроса...";

    if (!query.exec()) {
        qDebug() << "ОШИБКА SQL:" << query.lastError().text();
        qDebug() << "SQL запрос:" << query.lastQuery();
        qDebug() << "Значения:" << event.title << event.event_datetime << event.location << event.max_participants << event.status;
        return false;
    }

    qDebug() << "УСПЕХ: Мероприятие добавлено, ID:" << query.lastInsertId();
    return true;
}

QVector<Event> DatabaseManager::getAllEvents()
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

bool DatabaseManager::updateEvent(const Event &event)
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

bool DatabaseManager::deleteEvent(int event_id)
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

Event DatabaseManager::getEventById(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT event_id, title, description, event_datetime, location, max_participants, status, created_date FROM Events WHERE event_id = ?");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return Event(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toInt(),
            query.value(6).toString(),
            query.value(7).toString()
        );
    }

    return Event();
}

// Реализации методов работы с участниками
bool DatabaseManager::addParticipant(const Participant &participant)
{
    qDebug() << "=== DatabaseManager::addParticipant ===";
    qDebug() << "Данные участника:";
    qDebug() << "  Name:" << participant.full_name;
    qDebug() << "  Email:" << participant.email;
    qDebug() << "  Phone:" << participant.phone;

    QSqlQuery query;
    query.prepare("INSERT INTO Participants (full_name, email, phone) VALUES (?, ?, ?)");
    query.addBindValue(participant.full_name);
    query.addBindValue(participant.email);
    query.addBindValue(participant.phone);

    if (!query.exec()) {
        qDebug() << "ОШИБКА добавления участника:" << query.lastError().text();
        return false;
    }

    qDebug() << "УСПЕХ: Участник добавлен, ID:" << query.lastInsertId();
    return true;
}

QVector<Participant> DatabaseManager::getAllParticipants()
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

bool DatabaseManager::updateParticipant(const Participant &participant)
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

Participant DatabaseManager::getParticipantById(int participant_id)
{
    QSqlQuery query;
    query.prepare("SELECT participant_id, full_name, email, phone, reg_date FROM Participants WHERE participant_id = ?");
    query.addBindValue(participant_id);

    if (query.exec() && query.next()) {
        return Participant(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString()
        );
    }

    return Participant();
}

// Реализации методов работы с регистрациями
bool DatabaseManager::addRegistration(const Registration &registration)
{
    qDebug() << "=== DatabaseManager::addRegistration ===";
    qDebug() << "Данные регистрации:";
    qDebug() << "  Participant ID:" << registration.participant_id;
    qDebug() << "  Event ID:" << registration.event_id;
    qDebug() << "  Status:" << registration.participation_status;

    QSqlQuery query;
    query.prepare("INSERT INTO Registrations (participant_id, event_id, participation_status) VALUES (?, ?, ?)");
    query.addBindValue(registration.participant_id);
    query.addBindValue(registration.event_id);
    query.addBindValue(registration.participation_status);

    if (!query.exec()) {
        qDebug() << "ОШИБКА добавления регистрации:" << query.lastError().text();
        return false;
    }

    qDebug() << "УСПЕХ: Регистрация добавлена, ID:" << query.lastInsertId();
    return true;
}

QVector<Registration> DatabaseManager::getRegistrationsForEvent(int event_id)
{
    QVector<Registration> registrations;
    QSqlQuery query;
    query.prepare("SELECT r.registration_id, r.participant_id, r.event_id, r.participation_status, r.reg_date, "
                  "p.full_name FROM Registrations r "
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
            reg.participant_name = query.value(5).toString();

            registrations.append(reg);
        }
    }

    return registrations;
}

int DatabaseManager::getParticipantCountForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Registrations WHERE event_id = ? AND participation_status != 'cancelled'");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

bool DatabaseManager::updateRegistrationStatus(int registration_id, const QString &status)
{
    QSqlQuery query;
    query.prepare("UPDATE Registrations SET participation_status = ? WHERE registration_id = ?");
    query.addBindValue(status);
    query.addBindValue(registration_id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления статуса регистрации:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// Реализации методов работы с бюджетом
bool DatabaseManager::addBudgetItem(const BudgetItem &budgetItem)
{
    qDebug() << "=== DatabaseManager::addBudgetItem ===";
    qDebug() << "Данные бюджета:";
    qDebug() << "  Event ID:" << budgetItem.event_id;
    qDebug() << "  Expense:" << budgetItem.expense_item;
    qDebug() << "  Planned:" << budgetItem.planned_amount;
    qDebug() << "  Actual:" << budgetItem.actual_amount;

    QSqlQuery query;
    query.prepare("INSERT INTO Budget (event_id, expense_item, planned_amount, actual_amount) VALUES (?, ?, ?, ?)");
    query.addBindValue(budgetItem.event_id);
    query.addBindValue(budgetItem.expense_item);
    query.addBindValue(budgetItem.planned_amount);
    query.addBindValue(budgetItem.actual_amount);

    if (!query.exec()) {
        qDebug() << "ОШИБКА добавления бюджета:" << query.lastError().text();
        return false;
    }

    qDebug() << "УСПЕХ: Бюджет добавлен, ID:" << query.lastInsertId();
    return true;
}

QVector<BudgetItem> DatabaseManager::getBudgetForEvent(int event_id)
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

double DatabaseManager::getTotalPlannedBudgetForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(planned_amount) FROM Budget WHERE event_id = ?");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

double DatabaseManager::getTotalActualBudgetForEvent(int event_id)
{
    QSqlQuery query;
    query.prepare("SELECT SUM(actual_amount) FROM Budget WHERE event_id = ?");
    query.addBindValue(event_id);

    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

bool DatabaseManager::updateBudgetItem(const BudgetItem &budgetItem)
{
    QSqlQuery query;
    query.prepare("UPDATE Budget SET expense_item = ?, planned_amount = ?, actual_amount = ? WHERE budget_id = ?");
    query.addBindValue(budgetItem.expense_item);
    query.addBindValue(budgetItem.planned_amount);
    query.addBindValue(budgetItem.actual_amount);
    query.addBindValue(budgetItem.budget_id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления статьи бюджета:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
