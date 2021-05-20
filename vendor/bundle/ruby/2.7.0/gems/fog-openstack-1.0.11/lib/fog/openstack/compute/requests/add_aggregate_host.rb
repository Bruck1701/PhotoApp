module Fog
  module OpenStack
    class Compute
      class Real
        def add_aggregate_host(uuid, host_uuid)
          data = {'add_host' => {'host' => host_uuid}}
          request(
            :body    => Fog::JSON.encode(data),
            :expects => [200],
            :method  => 'POST',
            :path    => "os-aggregates/#{uuid}/action"
          )
        end
      end

      class Mock
        def add_aggregate_host(_uuid, _host_uuid)
          response = Excon::Response.new
          response.status = 200
          response.headers = {
            "Content-Type"   => "text/html; charset=UTF-8",
            "Content-Length" => "0",
            "Date"           => Date.new
          }
          response
        end
      end
    end
  end
end
