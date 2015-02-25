@filename = "MovingNQueens"

task :default do
  `g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp`
end

task :run do
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  system("java -jar ./#{@filename}Vis.jar -seed 1 -exec './#{@filename}'")
  #system("java -jar ./#{@filename}Vis.jar -side 12 -seed 105 -exec './#{@filename}'")
end

task :one do
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  system("time java -jar #{@filename}Vis.jar -seed 2 -novis -exec './#{@filename}'")
end

task :two do
  system("g++ -o #{@filename} #{@filename}.cpp")
  system("time java -jar #{@filename}Vis.jar -seed 8 -novis -exec './#{@filename}'")
end

task :novis do
  system('rm result.txt')
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  1.upto(100) do |num|
    p num
    system("time java -jar ./#{@filename}Vis.jar -seed #{num} -novis -exec './#{@filename}' >> result.txt")
    sleep(0.7)
  end
  system('ruby analysis.rb 100')
end

task :final do
  system('rm result.txt')
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  1.upto(1000) do |num|
    p num
    system("time java -jar ./#{@filename}Vis.jar -seed #{num} -novis -exec './#{@filename}' >> result.txt")
    sleep(1.0)
  end
  system('ruby analysis.rb 1000')
end

task :sample do
  system('rm result.txt')
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  1.upto(10) do |num|
    p num
    system("time java -jar ./#{@filename}Vis.jar -seed #{num} -novis -exec './#{@filename}' >> result.txt")
  end
  system('ruby analysis.rb 10')
end

task :select do
  system('rm result.txt')
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  array = [1, 8, 11, 16, 17, 20, 37, 39, 45, 49, 54, 57, 58, 59, 60, 64, 65, 67, 74, 79, 81, 83, 86, 91, 93, 94, 95, 96, 103, 105, 108, 109, 110, 118, 119, 123, 127, 131, 136, 141, 143, 146, 148, 150, 153, 161, 163, 168, 169, 171, 173, 174, 184, 186, 189, 191, 193, 194, 195, 198, 199]
  array.each do |num|
    p num
    system("time java -jar ./#{@filename}Vis.jar -seed #{num} -novis -exec './#{@filename}' >> result.txt")
    sleep(0.6)
  end
  system("ruby analysis.rb #{array.size}")
end

task :test do
  system("g++ -o #{@filename} #{@filename}.cpp")
  system("./#{@filename} < test_case.txt")
end

task :java do
  system("g++ -W -Wall -Wno-sign-compare -O2 -o #{@filename} #{@filename}.cpp")
  system("java CirclesSeparationVis -seed 87 -exec './CirclesSeparation'")
end
