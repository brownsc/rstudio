/*
 * SocketRpc.cpp
 *
 * Copyright (C) 2017 by RStudio, Inc.
 *
 * Unless you have received this program directly from RStudio pursuant
 * to the terms of a commercial license agreement with RStudio, then
 * this program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */

#include <core/Error.hpp>
#include <core/json/Json.hpp>
#include <core/json/JsonRpc.hpp>
#include <core/http/LocalStreamBlockingClient.hpp>
#include <core/http/TcpIpBlockingClient.hpp>
#include <core/http/TcpIpBlockingClientSsl.hpp>
#include <core/SafeConvert.hpp>
#include <core/SocketRpc.hpp>
#include <core/system/Crypto.hpp>
#include <core/system/Environment.hpp>

namespace rstudio {
namespace core {
namespace socket_rpc {

namespace {

std::string s_sessionSharedSecret;

void constructRequest(const std::string& endpoint,
                      const json::Object& payload,
                      http::Request* pRequest)
{
   // serialize the payload
   std::ostringstream oss;
   core::json::write(payload, oss);

   // form the request
   pRequest->setMethod("POST");
   pRequest->setUri(endpoint);
   pRequest->setHeader("Connection", "close");
   pRequest->setBody(oss.str());
}

Error handleResponse(const std::string& endpoint,
                     const http::Response& response,
                     json::Value* pResult)
{
   if (response.statusCode() != core::http::status::Ok)
   {
      LOG_WARNING_MESSAGE("Server RPC failed: " + endpoint + " " +
                          safe_convert::numberToString(response.statusCode()) +
                          " " + response.statusMessage() + "\n" +
                          response.body());
      return Error(json::errc::ExecutionError, ERROR_LOCATION);
   }
   else if (response.body().empty())
   {
      // empty value from server doesn't imply failure, just that there's
      // nothing for us to read
      *pResult = json::Value();
      return Success();
   }
   else if (!json::parse(response.body(), pResult))
   {
      LOG_WARNING_MESSAGE("Received unparseable result from rserver RPC:\n" +
            endpoint + "\n" +
            response.body());
      return Error(json::errc::ParseError, ERROR_LOCATION);
   }

   return Success();
}

Error sendRequest(const FilePath& socketPath,
                  const std::string& endpoint,
                  const http::Request& request,
                  json::Value* pResult)
{
   core::http::Response response;

   Error error = core::http::sendRequest(socketPath, request, &response);
   if (error)
      return error;

   return handleResponse(endpoint, response, pResult);
}

Error sendRequest(const std::string& address,
                  const std::string& port,
                  bool ssl,
                  const std::string& endpoint,
                  const http::Request& request,
                  json::Value* pResult)
{
   core::http::Response response;

   Error error;
   if (ssl)
      error = core::http::sendSslRequest(address, port, true, request, &response);
   else
      error = core::http::sendRequest(address, port, request, &response);

   if (error)
      return error;

   return handleResponse(endpoint, response, pResult);
}

} // anonymous namespace

Error invokeRpc(const FilePath& socketPath,
                const std::string& endpoint,
                const json::Object& request,
                json::Value *pResult)
{
   http::Request req;
   constructRequest(endpoint, request, &req);

   // stamp rpc secret key on header
   // only used with unix sockets
   req.setHeader(kServerRpcSecretHeader, s_sessionSharedSecret);

   return sendRequest(socketPath, endpoint, req, pResult);
}

Error invokeRpc(const std::string& address,
                const std::string& port,
                bool ssl,
                const std::string& endpoint,
                const json::Object& request,
                json::Value *pResult)
{
   http::Request req;
   constructRequest(endpoint, request, &req);

   // stamp auth cookie on the request
   // this lets the server know the RPC is coming from a trusted source,
   // and on behalf of which user
   req.setHeader(kRstudioRpcCookieHeader, core::system::getenv(kRstudioRpcCookieEnvVar));

   // add additional Host header (needed for tcp connections)
   req.setHost(address);

   return sendRequest(address, port, ssl, endpoint, req, pResult);
}

Error initialize()
{
   // extract shared secret
   s_sessionSharedSecret = core::system::getenv(kServerRpcSecretEnvVar);
   core::system::unsetenv(kServerRpcSecretEnvVar);

   return Success();
}

Error initializeSecret(const std::string& rpcSecret)
{
   s_sessionSharedSecret = rpcSecret;

   return Success();
}

const std::string& secret()
{
   return s_sessionSharedSecret;
}

} // namespace socket_rpc
} // namespace core
} // namespace rstudio

