// Created by author ethereal on 2024/12/20.
// Copyright (c) 2024 ethereal. All rights reserved.
#include "InetAddress.hpp"

namespace common::net
{
InetAddress::InetAddress(const boost::asio::ip::address_v6::bytes_type& addr) {
	ip_address_ = boost::asio::ip::address_v6(addr);
}

InetAddress::InetAddress(const std::string& host) {
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::resolver resolver(io_context);
	const boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(host, "");
	ip_address_ = results->endpoint().address();
}

/// \brief Returns the address of the InetAddress object as a vector of bytes.
/// \details Depending on whether the address is an IPv4 or IPv6 address, the size of the returned vector will be 4 bytes
/// (for IPv4) or 16 bytes (for IPv6).
auto InetAddress::getAddress() const -> std::vector<unsigned char> {
	std::vector<unsigned char> result;
	if (ip_address_.is_v4()) {
		const boost::asio::ip::address_v4 v4_address = ip_address_.to_v4();
		result.assign(v4_address.to_bytes().begin(), v4_address.to_bytes().end());
	}
	else if (ip_address_.is_v6()) {
		const boost::asio::ip::address_v6 v6_address = ip_address_.to_v6();
		result.assign(v6_address.to_bytes().begin(), v6_address.to_bytes().end());
	}
	return result;
}

/// \brief Returns the address of the InetAddress object as a string.
/// \details Depending on whether the address is an IPv4 or IPv6 address, the returned string will be in the format of
/// "xx.xx.xx.xx" or "[xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx]".
auto InetAddress::getHostAddress() const -> std::string {
	return ip_address_.to_string();
}

/// \brief Retrieves the local host address.
/// \details This function returns an InetAddress object representing the loopback address, which is typically used
/// to refer to the local host in networking contexts. The loopback address is an IPv6 address in this implementation.
auto InetAddress::getLocalHost() -> InetAddress {
	const boost::asio::ip::address_v6 addr = boost::asio::ip::address_v6::loopback();
	return InetAddress(addr.to_bytes());
}

/// \brief Compares this InetAddress object with another.
/// \details This function compares the addresses of this and the other InetAddress objects to determine if they are equal.
/// \param other The InetAddress object to compare with.
/// \return true if the addresses are equal, false otherwise.
auto InetAddress::equals(const InetAddress& other) const -> bool {
	return ip_address_ == other.ip_address_;
}

/// \brief Returns the canonical hostname of the InetAddress object.
/// \details This function returns the canonical hostname representation of the current IP address.
/// The canonical hostname is the fully qualified domain name (FQDN) of the host that the InetAddress object represents.
/// \return A string representing the canonical hostname of the InetAddress object.
auto InetAddress::getCanonicalHostName() const -> std::string {
	return ip_address_.to_string();
}

/// \brief Retrieves the hostname associated with the IP address.
/// \details This function returns the hostname representation of the current IP address.
/// It uses the address conversion functionality provided by boost::asio to obtain the hostname.
/// \return A string representing the hostname of the InetAddress object.
auto InetAddress::getHostName() const -> std::string {
	return ip_address_.to_string();
}

/// \brief Returns a hash code value for this InetAddress object.
/// \details The returned hash code is based on the IP address of the InetAddress object.
/// \return A size_t value representing the hash code of the InetAddress object.
auto InetAddress::hashCode() const -> size_t {
	return std::hash<std::string>{}(ip_address_.to_string());
}

/// \brief Checks if the InetAddress object is an IP loopback address.
/// \details This function returns true if the InetAddress object is a loopback address,
/// false otherwise.
auto InetAddress::isLoopbackAddress() const -> bool {
	return ip_address_.is_loopback();
}

/// \brief Checks if the InetAddress object is a multicast address.
/// \details This function returns true if the InetAddress object is a multicast address,
/// false otherwise.
/// \return true if the address is a multicast address, false otherwise.
auto InetAddress::isMulticastAddress() const -> bool {
	return ip_address_.is_multicast();
}

/// \brief Checks if the InetAddress object is reachable within a specified time limit.
/// \details This function attempts to establish a connection to the InetAddress object
/// within a specified time limit. The InetAddress object is considered reachable if
/// the connection is established before the time limit is reached.
/// \param timeout_ms The time limit in milliseconds. The connection attempt will be
/// aborted if the connection is not established before this time.
/// \return true if the InetAddress object is reachable, false otherwise.
auto InetAddress::isReachable(int timeout_ms) const -> bool {
	try {
		boost::asio::io_context io_context;
		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::steady_timer timer(io_context);
		timer.expires_from_now(std::chrono::milliseconds(timeout_ms));
		boost::asio::ip::tcp::endpoint endpoint(ip_address_, 80);
		socket.async_connect(endpoint, [&](const boost::system::error_code& ec) {
			if (!ec) {
				std::cout << "Connection established to " << ip_address_.to_string() << std::endl;
			}
			else {
				std::cout << "Failed to connect: " << ec.message() << std::endl;
			}
		});
		io_context.run_for(std::chrono::milliseconds(timeout_ms));
		return socket.is_open();
	}
	catch (const std::exception& e) {
		std::cout << "Error in isReachable: " << e.what() << std::endl;
		return false;
	}
}

/// \brief Converts the InetAddress object to a string representation.
/// \details The returned string contains the IP address in the standard IPv4 or IPv6 format.
/// \return A string representing the InetAddress object.
auto InetAddress::toString() const -> std::string {
	return ip_address_.to_string();
}
}
