def lc(f)
	File.read(f).count("\n")
end

@lc = 0
@fc = Dir.glob("ESEP/src/**/*.{h,cpp}").each do |f|
	@lc += lc(f)
end.length

puts "ESEP project contains #{@fc} files totalling #{@lc} lines of code."

