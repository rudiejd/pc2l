#ifndef HTTP_FILE_H
#define HTTP_FILE_H

/**
 * A custom class to streamline sending file contents to a given
 * output stream.  This streams the data in HTTP format.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */
#include <string>
#include <iostream>

/** A HTTP namespace to disambiguate the file class encapsulated by
    it.
*/
namespace http {
    /**
     * The defaut HTTP headers used when sending file contents to a
     * web-client.  NOTE: The content-type header must always be the
     * last in the set of headers so that this class can easily write
     * content-type.
     */
    const std::string DefaultHttpHeaders =
        "HTTP/1.1 200 OK\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Connection: Close\r\n"
        "Content-Type: ";

    /**
     * A fixed HTTP-404 error HTTP header that is sent to the client
     * if a specified file was not found on the server.
     */
    const std::string Http404Headers =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Connection: Close\r\n\r\n";

    /** Convenience wrapper class to stream path to a given file as a
        HTTP response.
    */
    class file {
    public:
        // Stream insertion operator to ease streaming contents of a
        // file to a given output stream.
        friend std::ostream& operator<<(std::ostream& os,
                                        const http::file& file);

        /** The constructor that merely saves the parameters to
            instance variables. The actual streaming
            of the file is done when operator<< is invoked.

            \param[in] path The path to the file to be streamed out by
            this class. By default the path is assumed to be
            index.html.  If this file does not exist, then this method
            sends out a 404 file not found HTTP response.

            \param[in] headers An optional set to 
        */
        file(const std::string& path = "index.html",
             const std::string& headers = DefaultHttpHeaders) :
            path(path), headers(headers) {}

    private:
        /**
         * Path to the file to be streamed out by this class. This
         * value is set in the constructor and is never changed.
         */
        std::string path;

        /**
         * The 200-OK HTTP headers to be be streamed out by this
         * class. This value is set in the constructor and is never
         * changed.
         */        
        std::string headers;
    };

    /**
     * Convenience method to determine the HTTP content-type based on
     * the file's extension. Specifically, this method extracts the
     * extension (e.g, ".html" form "index.html") and returns the HTTP
     * mime-type (e.g., "text/html") based on the file's extension.
     *
     * \param[in] path The path to the file whose extension is to be
     * used to determine the content type.
     *
     * \return The content type associated with the file. By default
     * this method returns "text/plain" as the content type.
     */
    std::string getContentType(const std::string& path);

    /** Prototype for operator<< used for HTTP-streaming file contents */
    std::ostream& operator<<(std::ostream& os, const file& file);
    
};  // namespace http

#endif
