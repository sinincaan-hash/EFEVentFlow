#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QDebug>

// Структуры для хранения данных
struct Event {
    int id;
    QString title;
    QString description;
    QString event_datetime;
    QString location;
    int max_participants;
    QString status;
    QString created_date;

    Event(int id = 0, const QString &title = "", const QString &description = "",
          const QString &event_datetime = "", const QString &location = "",
          int max_participants = 0, const QString &status = "planned",
          const QString &created_date = "")
        : id(id), title(title), description(description), event_datetime(event_datetime),
          location(location), max_participants(max_participants), status(status),
          created_date(created_date) {}
};

struct Participant {
    int id;
    QString full_name;
    QString email;
    QString phone;
    QString reg_date;

    Participant(int id = 0, const QString &full_name = "", const QString &email = "",
                const QString &phone = "", const QString &reg_date = "")
        : id(id), full_name(full_name), email(email), phone(phone), reg_date(reg_date) {}
};

struct Registration {
    int registration_id;
    int participant_id;
    int event_id;
    QString participation_status;
    QString reg_date;
    QString participant_name; // Для отображения

    Registration(int registration_id = 0, int participant_id = 0, int event_id = 0,
                 const QString &participation_status = "registered", const QString &reg_date = "")
        : registration_id(registration_id), participant_id(participant_id), event_id(event_id),
          participation_status(participation_status), reg_date(reg_date) {}
};

struct BudgetItem {
    int budget_id;
    int event_id;
    QString expense_item;
    double planned_amount;
    double actual_amount;
    QString date_created;

    BudgetItem(int budget_id = 0, int event_id = 0, const QString &expense_item = "",
               double planned_amount = 0.0, double actual_amount = 0.0,
               const QString &date_created = "")
        : budget_id(budget_id), event_id(event_id), expense_item(expense_item),
          planned_amount(planned_amount), actual_amount(actual_amount),
          date_created(date_created) {}
};

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager& instance();

    // Методы для работы с базой данных
    bool initDatabase();
    bool createTables();

    // Методы для работы с событиями
    bool addEvent(const Event &event);
    QVector<Event> getAllEvents();
    bool updateEvent(const Event &event);
    bool deleteEvent(int event_id);
    Event getEventById(int event_id);

    // Методы для работы с участниками
    bool addParticipant(const Participant &participant);
    QVector<Participant> getAllParticipants();
    bool updateParticipant(const Participant &participant);
    Participant getParticipantById(int participant_id);

    // Методы для работы с регистрациями
    bool addRegistration(const Registration &registration);
    QVector<Registration> getRegistrationsForEvent(int event_id);
    int getParticipantCountForEvent(int event_id);
    bool updateRegistrationStatus(int registration_id, const QString &status);

    // Методы для работы с бюджетом
    bool addBudgetItem(const BudgetItem &budgetItem);
    QVector<BudgetItem> getBudgetForEvent(int event_id);
    double getTotalPlannedBudgetForEvent(int event_id);
    double getTotalActualBudgetForEvent(int event_id);
    bool updateBudgetItem(const BudgetItem &budgetItem);

private:
    DatabaseManager() {}
    ~DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
