class Summary
  def initialize(num = 100)
    sum = 0 
    cnt = 0
    File.open(Dir::pwd + '/result.txt', 'r') do |file|
      file.readlines.each do |line|
        if line =~ /Score/
          score = line.chomp.split(' ')[2].to_f

          if score.zero?
            p cnt
            score = Float::INFINITY
          end
          sum += score
          cnt += 1
        end 
      end 
    end 
    p sum/cnt.to_f
  end 
end

s = Summary.new(ARGV[0])
