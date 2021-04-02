require 'openssl'
require 'base64'
require 'aws-sdk-cognitoidentityprovider'

USER_CREATE_NEW      = 0x01
USER_CREATE_BAD_PASS = 0x02
USER_CREATE_EXISTS   = 0x04
USER_CREATE_UNKNOWN  = 0x08
AUTH_SUCCESS         = 0x01
AUTH_BAD_PASS        = 0x02
AUTH_UNKNOWN         = 0x04

def get_secret_hash(username)
  msg = username + ENV['CLIENT_ID']
  Base64.encode64(
    OpenSSL::HMAC.digest(
      OpenSSL::Digest.new('sha256'),
      get_client_secret,
      msg
    )
  ).strip
end

def get_client_secret
  client = Aws::CognitoIdentityProvider::Client.new(region: ENV['AWS_REGION'])
  client.describe_user_pool_client({
    user_pool_id: ENV['USER_POOL_ID'],
    client_id: ENV['CLIENT_ID']
  }).user_pool_client.client_secret
end

def sign_up(username, password)
  client = Aws::CognitoIdentityProvider::Client.new(region: ENV['AWS_REGION'])
  begin
    puts resp = client.sign_up({
      client_id: ENV['CLIENT_ID'],
      secret_hash: get_secret_hash(username),
      username: username,
      password: password
    })
    return {code: USER_CREATE_NEW, msg: "new user #{username} created"}
  rescue Aws::CognitoIdentityProvider::Errors::InvalidPasswordException => e
    return {code: USER_CREATE_BAD_PASS, msg: e.message}
  rescue Aws::CognitoIdentityProvider::Errors::UsernameExistsException => e
    return {code: USER_CREATE_EXISTS, msg: e.message}
  rescue Exception => e
    return {code: USER_CREATE_UNKNOWN, msg: e.message}
  end
end

def challenge_auth(username, password)
  client = Aws::CognitoIdentityProvider::Client.new(region: ENV['AWS_REGION'])
  begin
    resp = client.admin_initiate_auth(
      user_pool_id: ENV['USER_POOL_ID'],
      client_id: ENV['CLIENT_ID'],
      auth_flow: 'ADMIN_NO_SRP_AUTH',
      auth_parameters: {
        USERNAME: username,
        SECRET_HASH: get_secret_hash(username),
        PASSWORD: password
      },
      client_metadata: {
        'username': username,
        'password': password
      }
    ).to_h
    return {auth: 'success', msg: resp[:authentication_result][:id_token], response: resp}
  rescue Aws::CognitoIdentityProvider::Errors::NotAuthorizedException => e
    return {auth: 'failed', msg: e.message}
  rescue Exception => e
    return {auth: 'failed', msg: e.message}
  end
end


def lambda_handler(event:, context:)
  puts event

  case event['path']
  when '/create_user'
    body = JSON.parse(event['body'])
    puts "Initilizing user: #{body['username']}..."
    create_user_ret = sign_up(body['username'], body['password'])
    if create_user_ret[:code] & (USER_CREATE_NEW | USER_CREATE_EXISTS) > 0
      puts create_user_ret[:code]
      user_auth = challenge_auth(body['username'], body['password'])
    else
      puts ret = {statusCode: 403, body: create_user_ret.to_json}
      return ret
    end
    puts ret = {statusCode: 200, body: user_auth.to_json}
    return ret
  end
end
