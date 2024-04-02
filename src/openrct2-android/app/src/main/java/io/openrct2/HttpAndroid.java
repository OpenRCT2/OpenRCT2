package io.openrct2;

import android.util.Log;

import org.apache.commons.io.IOUtils;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.HttpURLConnection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class HttpAndroid {
    private static final String TAG = "HttpAndroid";

    // Corresponding Java enum for the C++ 'Status' enum
    public enum Status {
        Invalid(0),
        Ok(200),
        NotFound(404);

        private final int code;
        Status(int code) {
            this.code = code;
        }

        public int getCode() {
            return code;
        }
    }

    // Corresponding Java enum for the C++ 'Method' enum
    public enum Method {
        GET,
        POST,
        PUT
    }

    // Java class equivalent to the C++ 'Response' struct
    public static class Response {
        Status status;
        String contentType;
        String body;
        Map<String, List<String>> headers;
        String error;

    }

    // Java class equivalent to the C++ 'Request' struct
    public static class Request {
        String url;
        Map<String, String> headers;
        Method method;
        String body;
        boolean forceIPv4;

        public Request() {
            this.method = Method.GET; // Default method
        }
    }

    public static Response request(Request request) {
        Response response = new Response();
        response.status = Status.Invalid;
        response.error = "Request failed";
        try {
            InputStream inputStream = null;
            try {
                URL url = new URL(request.url);
                HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                connection.setRequestMethod(request.method.toString());
                connection.setConnectTimeout(10000);
                connection.setReadTimeout(10000);
                connection.setUseCaches(false);
                connection.setDoInput(true);
                connection.setInstanceFollowRedirects(true);
                if (request.headers != null) {
                    for (Map.Entry<String, String> entry : request.headers.entrySet()) {
                        connection.setRequestProperty(entry.getKey(), entry.getValue());
                    }
                }
                connection.connect();
                if (request.body!= null) {
                    OutputStream os = connection.getOutputStream();
                    os.write(request.body.getBytes());
                    os.flush();
                    os.close();
                }
                int responseCode = connection.getResponseCode();
                if (responseCode == 200) {
                    inputStream = connection.getInputStream();
                } else {
                    inputStream = connection.getErrorStream();
                }
                // Log
                Log.d(TAG, "Request: " + request.url + ", response code: " + responseCode);
                response.status = Status.Ok;
                response.contentType = connection.getContentType();
                response.headers = connection.getHeaderFields();
                response.body = IOUtils.toString(inputStream, "UTF-8");
                response.error = null;
                return response;
            } catch (IOException e) {
                Log.e(TAG, "Error while requesting " + request.url + ", error: " + e.getMessage(), e);
                response.error = e.getMessage();
                return response;
            } finally {
                if (inputStream != null) {
                    inputStream.close();
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "Error while requesting " + request.url, e);
            response.error = e.getMessage();
            return response;
        }
    }
}
