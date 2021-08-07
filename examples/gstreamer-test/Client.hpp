#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;

class client
{
    public:
        client(boost::asio::io_context& io_context,
               const std::string& server, const std::string& path);

    private:
        void handle_resolve(const boost::system::error_code& err,
                            const tcp::resolver::results_type& endpoints);

        void handle_connect(const boost::system::error_code& err);

        void handle_write_request(const boost::system::error_code& err);

        void handle_read_status_line(const boost::system::error_code& err);

        void handle_read_headers(const boost::system::error_code& err);

        void handle_read_content(const boost::system::error_code& err);

        tcp::resolver resolver_;
        tcp::socket socket_;
        boost::asio::streambuf request_;
        boost::asio::streambuf response_;
};

#endif /* __CLIENT_HPP__ */