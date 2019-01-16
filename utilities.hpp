#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <QDebug>
#include <Wt/Dbo/Dbo.h>
#include <Wt/WDateTime.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WDateTime.h>
#include <QDateTime>
#include <botan/bcrypt.h>
#include <string>
#include <memory>
#include <botan/auto_rng.h>

#define MAX_TRIES 3

namespace utilities
{
namespace dbo = Wt::Dbo;

template<typename T>
bool IsEmpty( T const & str ){ return str.isEmpty(); }

template<typename T, typename... U>
bool IsEmpty( T const & str, U &&...other_str )
{
    return IsEmpty( str ) || IsEmpty( std::forward<U>( other_str )... );
}

Wt::WDateTime QtDateTimeToWtDateTime( QDateTime const & object );

enum Permission
{
    CAN_CHECK_MAIL,
    CAN_ADD_STAFF =         0x1,
    CAN_CHECK_REPORTS =     0x2,
    CAN_GENERATE_REPORTS =  0x4,
    CAN_ADD_GUESTS =        0x8,
    CAN_CHECK_RESERVATIONS =0x10,
    CAN_ADD_UPDATE_RESERVATIONS = 0x20,
    CAN_ADD_EXPENSES =      0x40,
    CAN_UPDATE_REPORTS =    0x80,
    CAN_UPDATE_PASSWORDS =  0x100,
    CAN_SU                =  0x200
};

struct Lodging;

struct Order {
    std::string _item_name;
    std::string _amount;
    dbo::ptr<Lodging> lodging;
public:
    Order( std::string const & item_name, std::string const & amount );
    Order();
    template<typename Action>
    void persist( Action & action )
    {
        dbo::field( action, _item_name, "item_name" );
        dbo::field( action, _amount, "amount" );
        dbo::belongsTo( action, lodging, "lodging" );
    }
    static char const* table_name;
};

struct Guest;
struct Lodging
{
    long            lodging_ref_id; // reference id, primary key
    std::string     room_number;
    std::string     amount_paid;
    std::string     account_settler;

    std::string     arrived_from;
    std::string     departing_to;
    std::string     vehicle_license_plate;
    std::string     nature_of_visit;

    Wt::WDateTime   date_of_arrival;
    Wt::WDateTime   date_of_departure;
    long            number_of_nights;
    long            number_of_adults;
    long            number_of_children;

    dbo::ptr<Guest> guest;
    dbo::collection<dbo::ptr<Order>> orders;
public:

    template<typename Action>
    void persist( Action & action )
    {
        dbo::field( action, lodging_ref_id, "reference_id" );
        dbo::field( action, room_number, "room_number" );
        dbo::field( action, amount_paid, "amount_paid" );
        dbo::field( action, account_settler, "account_settler" );

        dbo::field( action, arrived_from, "arrived_from" );
        dbo::field( action, departing_to, "departing_to" );
        dbo::field( action, vehicle_license_plate, "vehicle_plate" );
        dbo::field( action, nature_of_visit, "nature_of_visit" );

        dbo::field( action, date_of_arrival, "arrival_date" );
        dbo::field( action, date_of_departure, "departure_date" );
        dbo::field( action, number_of_nights, "nights" );
        dbo::field( action, number_of_adults, "adults" );

        dbo::field( action, number_of_children, "children" );
        dbo::belongsTo( action, guest, "guest" );
        dbo::hasMany( action, orders, dbo::ManyToOne, "lodging" );
    }
    static char const* table_name;
};

struct Guest
{
    long        guest_id; //reference id, primary key
    std::string surname;
    std::string other_names;
    std::string nationality;
    std::string occupation;
    std::string address;
    std::string phone_number;
    std::string means_of_id;
    std::string nok_fullname; // next of kin(nok)
    std::string nok_address;
    std::string nok_phone_number;
    std::string picture_path;
    std::string vehicle_license_plate;

    dbo::collection<dbo::ptr<Lodging>> lodgings;

public:
    template<typename Action>
    void persist( Action & action )
    {
        dbo::field( action, guest_id, "guest_id" );
        dbo::field( action, surname, "surname" );
        dbo::field( action, other_names, "other_names" );
        dbo::field( action, nationality, "nationality" );
        dbo::field( action, occupation, "occupation" );
        dbo::field( action, address, "address" );
        dbo::field( action, phone_number, "phone_number" );
        dbo::field( action, means_of_id, "means_of_id" );
        dbo::field( action, nok_fullname, "nok_fullname" );
        dbo::field( action, nok_address, "nok_address" );
        dbo::field( action, nok_phone_number, "nok_mobile_number" );
        dbo::field( action, picture_path, "picture_path" );
        dbo::field( action, vehicle_license_plate, "license_plate" );
        dbo::hasMany( action, lodgings, dbo::ManyToOne, "guest" );
    }
    static char const* table_name;
};

struct Staff
{
    struct Role
    {
        std::string role_name;
        int         permission;
    };
    long        staff_id;
    std::string name;
    std::string username;
    std::string password_hash;
    std::string address;
    Role        assigned_role;

    template<typename Action>
    void persist( Action &action )
    {
        dbo::field( action, staff_id, "staff_id" );
        dbo::field( action, name, "name" );
        dbo::field( action, username, "username" );
        dbo::field( action, password_hash, "password_hash" );
        dbo::field( action, address, "address" );
        dbo::field( action, assigned_role.role_name, "role_name" );
        dbo::field( action, assigned_role.permission, "permission" );
    }
    static char const* table_name;
};

std::string get_password_hash( char const *password );
bool is_correct_password( std::string const & password, std::string const & password_hash );
std::unique_ptr<dbo::Session> CheckDatabaseAccess();
}
#endif // UTILITIES_HPP
