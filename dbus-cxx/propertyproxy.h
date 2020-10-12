/***************************************************************************
 *   Copyright (C) 2020 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include <dbus-cxx/enums.h>
#include <dbus-cxx/dbus-cxx-config.h>
#include <dbus-cxx/variant.h>
#include <sigc++/sigc++.h>
#include <memory>

#ifndef DBUSCXX_PROPERTYPROXY_H
#define DBUSCXX_PROPERTYPROXY_H
namespace DBus {

class InterfaceProxy;

/**
 * Base type of PropertyProxy to allow for storage in e.g. a vector.
 */
class PropertyProxyBase {
protected:
    PropertyProxyBase( std::string name, PropertyUpdateType update );

public:

    /**
     * Get the name of this propery.
     * @return
     */
    std::string name() const;

    /**
     * Get the value of this property as a Variant.
     *
     * @return
     */
    Variant variant_value() const;

    PropertyUpdateType update_type() const;

    /**
     * This signal is emitted whenever the property changes
     * @return
     */
    sigc::signal<void(Variant)> signal_generic_property_changed();

    /**
     * Set the value of this property.
     *
     * When used on a remote property(a proxy), this will attempt to set
     * the value on the remote object.  If the property is READONLY, this
     * acts as a No-op.
     *
     * When used on a local property(adapter), this will emit the PropertyChanged
     * DBus signal in order to notify clients that the property has updated.
     * Note that the exact value of the PropertyUpdateType will determine what is
     * emitted(invalidated, new value, or invalidation)
     *
     * @param value The new value to set
     */
    void set_value( Variant value );

    InterfaceProxy* interface_name() const;

private:
    void set_interface( InterfaceProxy* proxy );
    void updated_value( Variant value );

private:
    class priv_data;

    DBUS_CXX_PROPAGATE_CONST( std::unique_ptr<priv_data> ) m_priv;

    // Declare InterfaceProxy as a friend so that it can set the interface
    friend class InterfaceProxy;
};

/**
 * Represents a remote DBus property.
 *
 * Properties can be Read, Write, or Readonly.
 */
template <typename T_type>
class PropertyProxy : public PropertyProxyBase {
private:
    PropertyProxy( std::string name, PropertyUpdateType update ) :
        PropertyProxyBase( name, update ) {}

public:
    static PropertyProxy<T_type> create( std::string name, PropertyUpdateType update ) {
        return std::shared_ptr( new PropertyProxy<T_type>( name, update ) );
    }

    sigc::signal<void(T_type)> signal_property_changed() {
        return m_signal_changed;
    }

    void set_value( T_type t ) {
        set_value( Variant( t ) );
    }

    T_type value() const {
        T_type t = variant_value();
        return t;
    }

private:
    sigc::slot<void(T_type)> m_signal_changed;
};

} /* namespace DBus */

#endif /* DBUSCXX_PROPERTY_H */
