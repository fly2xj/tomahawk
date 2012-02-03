/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOMAHAWK_ACCOUNTS_ACCOUNTMODELNODE_H
#define TOMAHAWK_ACCOUNTS_ACCOUNTMODELNODE_H

#include "Account.h"
#include "AccountManager.h"
#include "ResolverAccount.h"

#include <attica/content.h>

namespace Tomahawk {

namespace Accounts {

/**
 * Node for account tree.
 *
 * Basically a union with possible types:
 * 1) AccountFactory* for all factories that have child accounts. Also a list of children
 * 2) Attica::Content for AtticaResolverAccounts (with associated AtticaResolverAccount*) (all synchrotron resolvers)
 * 3) ResolverAccount* for manually added resolvers (from file).
 *
 * These are the top-level items in tree.
 *
 * Top level nodes all look the same to the user. The only difference is that services that have login (and thus
 *  can have multiple logins at once) allow a user to create multiple children with specific login information.
 *  All other top level accounts (Account*, Attica::Content, ResolverAccount*) behave the same to the user, they can
 *  simply click "Install" or toggle on/off.
 *
 */

struct AccountModelNode {
    enum NodeType {
        FactoryType,
        UniqueFactoryType,
        AtticaType,
        ManualResolverType
    };
    AccountModelNode* parent;
    NodeType type;

    /// 1, 4
    AccountFactory* factory;
    QList< Account* > accounts; // list of children accounts (actually existing and configured accounts)

    /// 2.
    Attica::Content atticaContent;
    AtticaResolverAccount* atticaAccount;

    /// 3.
    ResolverAccount* resolverAccount;

    // Construct in one of four ways. Then access the corresponding members
    explicit AccountModelNode( AccountFactory* fac ) : type( FactoryType )
    {
        init();
        factory = fac;

        if ( fac->isUnique() )
            type = UniqueFactoryType;

        // Initialize factory nodes with their children
        foreach ( Account* acct,  AccountManager::instance()->accounts() )
        {
            if ( AccountManager::instance()->factoryForAccount( acct ) == fac )
            {
                qDebug() << "Found account for factory:" << acct->accountFriendlyName();
                accounts.append( acct );
            }
        }
    }

    explicit AccountModelNode( Attica::Content cnt ) : type( AtticaType )
    {
        init();
        atticaContent = cnt;

        qDebug() << "Creating attica model node for resolver:" << cnt.id();

        foreach ( Account* acct, AccountManager::instance()->accounts( Accounts::ResolverType ) )
        {
            if ( AtticaResolverAccount* resolver = qobject_cast< AtticaResolverAccount* >( acct ) )
            {
                if ( resolver->atticaId() == atticaContent.id() )
                {
                    qDebug() << "found atticaaccount :" << resolver->accountFriendlyName();
                    atticaAccount = resolver;
                    break;
                }
            }
        }
    }

    explicit AccountModelNode( ResolverAccount* ra ) : type( ManualResolverType )
    {
        init();
        resolverAccount = ra;
    }

    void init()
    {
        factory = 0;
        atticaAccount = 0;
        resolverAccount = 0;
    }
};

}

}
#endif // TOMAHAWK_ACCOUNTS_ACCOUNTMODELNODE_H
