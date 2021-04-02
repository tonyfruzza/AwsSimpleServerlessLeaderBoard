require 'aws-sdk-dynamodb'

def lambda_handler(event:, context:)
  @ddb = @ddb = Aws::DynamoDB::Client.new(region: ENV['AWS_REGION'])

  case event['path']
  when '/get_highscore'
    puts ret = {statusCode: 200, body: JSON.generate(@ddb.scan({table_name: ENV['DDB_TABLE']}).to_h[:items])}
    return ret
  when '/put_highscore'
    data = JSON.parse(event['body'])
    puts attr = {
      table_name: ENV['DDB_TABLE'],
      item: {
        gameId: data['game_id'],
        user: data['username'],
        score: data['score'].to_i,
        timestamp: Time.now.to_i
      }
    }
    ret = @ddb.put_item(attr).to_h
    puts ret = {statusCode: 200, body: JSON.generate(ret)}
    return ret
  else
    return {statusCode: 404, body: "Unknown route #{event['path']}"}
  end
end
