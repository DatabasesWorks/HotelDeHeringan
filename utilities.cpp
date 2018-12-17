#include "utilities.hpp"
#include <Wt/Dbo/Transaction.h>
#include <Wt/Dbo/Query.h>

namespace utilities
{
    namespace dbo = Wt::Dbo;

    Order::Order( std::string const & item_name, std::string const & amount ):
        _item_name( item_name ), _amount( amount ), lodging{}
    {
    }
    Order::Order(): Order{ "", "" }
    {
    }
    char const* Order::table_name = "orders";
    char const* Lodging::table_name = "lodgings";
    char const* Guest::table_name = "guests";
    char const* Staff::table_name = "staffs";

    std::string get_password_hash( char const *password )
    {
        std::unique_ptr<Botan::RandomNumberGenerator> pseudo_rng{ Botan::AutoSeeded_RNG::make_rng() };
        return Botan::generate_bcrypt( password, *pseudo_rng );
    }

    bool is_correct_password( std::string const & password, std::string const & password_hash )
    {
        return Botan::check_bcrypt( password, password_hash );
    }

    std::unique_ptr<dbo::Session> CheckDatabaseAccess()
    {
        auto overall_db = std::make_unique<dbo::backend::Sqlite3>( "blob.dat" );
        auto db_session = std::make_unique<dbo::Session>();

        using utilities::Lodging;
        using utilities::Guest;
        using utilities::Staff;
        using utilities::Order;

        db_session->setConnection( std::move( overall_db )  );
        db_session->mapClass<Lodging>( Lodging::table_name );
        db_session->mapClass<Guest>( Guest::table_name );
        db_session->mapClass<Staff>( Staff::table_name );
        db_session->mapClass<Order>( Order::table_name );

        try {
            db_session->createTables();
        }catch ( dbo::Exception & exception ){
            qDebug() << exception.what();
        }
        try{
            dbo::Transaction insert_transaction{ *db_session };
            dbo::Query<dbo::ptr<Staff>> admin = db_session->find<Staff>().where( "username = ?").bind( "sudo" );
            if( !admin.resultValue() ){
                auto const super_user_permissions = CAN_CHECK_MAIL | CAN_ADD_STAFF | CAN_CHECK_REPORTS |
                        CAN_GENERATE_REPORTS | CAN_ADD_GUESTS | CAN_CHECK_RESERVATIONS |
                        CAN_ADD_UPDATE_RESERVATIONS | CAN_ADD_EXPENSES | CAN_UPDATE_REPORTS |
                        CAN_UPDATE_PASSWORDS | CAN_SU;

                std::unique_ptr<Staff> super_user { new Staff() };
                super_user->name = "Super User";
                super_user->username = "sudo";
                super_user->password_hash = get_password_hash( "super_user" );
                super_user->address = "Lagos";
                super_user->assigned_role.role_name = "Super User";
                super_user->assigned_role.permission = super_user_permissions;

                auto result = db_session->add( std::move( super_user ));
                insert_transaction.commit();
            }
        } catch( dbo::Exception & exception ) {
            qDebug() << exception.what();
        }
        return db_session;
    }
}
#undef MAX_TRIES
