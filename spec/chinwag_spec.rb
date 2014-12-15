require 'chinwag'

describe Chinwag do
  # use latin here, because no entries include spaces
  before(:all) { @latin_dict = Chinwag::CWDict.open "latin" }

  it "generates letters" do
    amount = rand(1500..2500)
    result = Chinwag::generate(@latin_dict, :letters, amount, amount)

    expect(result.length).to eq(amount)
  end

  it "generates words" do
    amount = rand(1500..2500)
    result = Chinwag::generate(@latin_dict, :words, amount, amount)

    expect(result.split(" ").length).to eq(amount)
  end

  it "generates sentences" do
    amount = rand(1500..2500)
    result = Chinwag::generate(@latin_dict, :sentences, amount, amount)

    expect(result.split(/[\.!\?]/).length).to eq(amount)
  end

  it "generates paragraphs" do
    amount = rand(1500..2500)
    result = Chinwag::generate(@latin_dict, :paragraphs, amount, amount)

    expect(result.split(/\n\n/).length).to eq(amount)
  end
end

describe Chinwag::CWDict do
  before(:each) { @seuss_dict = Chinwag::CWDict.open "seussian" }

  it "successfully opens built-in dictionaries" do
    @latin_dict = Chinwag::CWDict.open "latin"

    expect(@seuss_dict).not_to be_nil
    expect(@seuss_dict).to be_named
    expect(@seuss_dict.name).to eq("seussian")
    expect(@seuss_dict.length).to be > 0
    expect(@seuss_dict.to_s).not_to eq("[]")

    expect(@latin_dict).not_to be_nil
    expect(@latin_dict).to be_named
    expect(@latin_dict.name).to eq("latin")
    expect(@latin_dict.length).to be > 0
    expect(@latin_dict.to_s).not_to eq("[]")
  end

  it "should have a name, and disclose it, and accepts a new name" do
    expect(@seuss_dict).to be_named
    expect(@seuss_dict.name).to eq("seussian")

    @seuss_dict.name = "some other name"

    expect(@seuss_dict).to be_named
    expect(@seuss_dict.name).to eq("some other name")
  end

  it "discloses its length/size" do
    expect(@seuss_dict.length).to eq(1096)
    expect(@seuss_dict.size).to eq(1096)
  end

  it "joins dictionary items by a connecting string (default: space)" do
    dir = File.dirname(__FILE__)

    file_name_1 = "chinwag_testcase_join_default"
    file_name_2 = "chinwag_testcase_join_comma"
    file_name_3 = "chinwag_testcase_join_hyphens"
    file_name_4 = "chinwag_testcase_join_dicks"

    file = File.open(File.join(dir, "testcases", file_name_1))
    test_case_default = file.read; file.close

    file = File.open(File.join(dir, "testcases", file_name_2))
    test_case_comma = file.read; file.close

    file = File.open(File.join(dir, "testcases", file_name_3))
    test_case_hyphens = file.read; file.close

    file = File.open(File.join(dir, "testcases", file_name_4))
    test_case_dicks = file.read; file.close

    expect(@seuss_dict.join).to eq(test_case_default)
    expect(@seuss_dict.join ",").to eq(test_case_comma)
    expect(@seuss_dict.join " -- ").to eq(test_case_hyphens)
    expect(@seuss_dict.join " 8===D ").to eq(test_case_dicks)
  end

  it "can sort itself (by entry length) into rows and disclose status" do
    @small_mess = Chinwag::CWDict.open
    @small_mess << %w[this is a quick test of sorting]

    expect(@small_mess.join).to eq("this test is of a quick sorting")
    expect(@small_mess).not_to be_sorted

    expect(@small_mess.sort!.join).to eq("a is of this test quick sorting")
    expect(@small_mess).to be_sorted
  end

  it "can clone/dup itself" do
    dir = File.dirname(__FILE__)
    file_name = "chinwag_testcase_seuss_dict_to_s"
    file = File.open(File.join(dir, "testcases", file_name))
    test_case = file.read; file.close

    clone = @seuss_dict.clone

    expect(@seuss_dict.to_s).to eq(test_case)
    expect(clone.to_s).to eq(test_case)

    @seuss_dict.close
    expect(@seuss_dict.to_s).to eq("[]")
    expect(clone.to_s).to eq(test_case)

    @seuss_dict = clone.dup

    expect(@seuss_dict.to_s).to eq(test_case)
    expect(clone.to_s).to eq(test_case)

    clone.close
    expect(@seuss_dict.to_s).to eq(test_case)
    expect(clone.to_s).to eq("[]")
  end

  it "should allow for pruning, removing duplicate entries" do
    flooder = %w[this is a string that will be duplicated]

    dir = File.dirname(__FILE__)

    file_name_1 = "chinwag_testcase_prune_original"
    file_name_2 = "chinwag_testcase_prune_pruned"

    file = File.open(File.join(dir, "testcases", file_name_1))
    test_case_original = file.read; file.close

    file = File.open(File.join(dir, "testcases", file_name_2))
    test_case_pruned = file.read; file.close

    @small_mess = Chinwag::CWDict.open

    @small_mess << flooder
    @small_mess << flooder
    @small_mess << flooder

    expect(@small_mess.to_s).to eq(test_case_original)
    expect(@small_mess.prune.to_s).to eq(test_case_pruned)
  end

  it "can be cleaned (pruned and sorted)" do
    flooder = %w[this is a string that will be duplicated]

    dir = File.dirname(__FILE__)

    file_name_1 = "chinwag_testcase_clean_original"
    file_name_2 = "chinwag_testcase_clean_cleaned"

    file = File.open(File.join(dir, "testcases", file_name_1))
    test_case_original = file.read; file.close

    file = File.open(File.join(dir, "testcases", file_name_2))
    test_case_cleaned = file.read; file.close

    @small_mess = Chinwag::CWDict.open

    @small_mess << flooder
    @small_mess << flooder
    @small_mess << flooder

    expect(@small_mess.to_s).to eq(test_case_original)
    expect(@small_mess.clean.to_s).to eq(test_case_cleaned)
  end

  it "can sample itself, pulling a single random entry" do
    sample = @seuss_dict.sample

    expect(sample.class).to be(String)
    expect(@seuss_dict.include? sample).to be true
  end

  it "checks its own validity" do
    @small_mess = Chinwag::CWDict.open
    @small_mess << %w[this is a quick test of validity]

    expect(@seuss_dict).to be_valid
    expect { @small_mess.valid? }.to raise_error(/dictionary too small/)

    for i in 0..300 do
      @small_mess << ["more", "test", "words"]
    end

    expect(@small_mess.length).to be > 300
    expect { @small_mess.valid? }.to raise_error(/couldn't be sorted/)
  end

  it "should be able to determine whether it has an entry or not" do
    @include_a = %w[this is a test of include]
    @exclude_a = %w[we are now testing exclude]

    @small_mess = Chinwag::CWDict.open
    @small_mess << @include_a

    @include_a.each { |i| expect(@small_mess.include? i).to be true }
    @include_a.each { |i| expect(@small_mess.exclude? i).to be false }

    @exclude_a.each { |e| expect(@small_mess.exclude? e).to be true }
    @exclude_a.each { |e| expect(@small_mess.include? e).to be false }
  end

  it "should be able to output a string representation of itself" do
    dir = File.dirname(__FILE__)
    file_name = "chinwag_testcase_seuss_dict_to_s"
    file = File.open(File.join(dir, "testcases", file_name))
    test_case = file.read; file.close

    expect(@seuss_dict.to_s).to eq(test_case)
  end

  it "should allow for the appending of entries" do
    @seuss_dict << %w[abcdefg hijklmn]

    expect(@seuss_dict.include? "abcdefg").to be true
    expect(@seuss_dict.include? "hijklmn").to be true

    @seuss_dict << "oqrstuv"

    expect(@seuss_dict.include? "oqrstuv").to be true

    @small_mess = Chinwag::CWDict.open
    
    expect(@small_mess.to_s).to eq("[]")

    @small_mess += %w[this is a quick test]

    expect(@small_mess.include? "this").to be true
    expect(@small_mess.include? "is").to be true
    expect(@small_mess.include? "a").to be true
    expect(@small_mess.include? "quick").to be true
    expect(@small_mess.include? "test").to be true

    @small_mess + %w[of appending strings]
    
    expect(@small_mess.include? "of").to be false
    expect(@small_mess.include? "appending").to be false
    expect(@small_mess.include? "strings").to be false
  end
end
