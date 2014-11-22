require 'sinatra/base'

class Server < Sinatra::Base
  get '/*' do |path|
    send_file File.join(settings.public_folder, path)
  end
end
