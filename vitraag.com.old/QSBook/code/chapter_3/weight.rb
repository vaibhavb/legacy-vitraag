

weight=[
{"wd"=>20,"bh"=>11,"bm"=>17,"ah"=>7,"am"=>47,"sm"=>481,"stm"=>3,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>21,"bh"=>12,"bm"=>28,"ah"=>6,"am"=>21,"sm"=>320,"stm"=>18,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>22,"bh"=>10,"bm"=>52,"ah"=>6,"am"=>34,"sm"=>440,"stm"=>4,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>23,"bh"=>11,"bm"=>30,"ah"=>6,"am"=>30,"sm"=>404,"stm"=>5,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>24,"bh"=>11,"bm"=>35,"ah"=>7,"am"=>00,"sm"=>425,"stm"=>7,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>25,"bh"=>11,"bm"=>27,"ah"=>6,"am"=>50,"sm"=>428,"stm"=>2,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>26,"bh"=>11,"bm"=>15,"ah"=>8,"am"=>00,"sm"=>506,"stm"=>7,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>27,"bh"=>11,"bm"=>13,"ah"=>7,"am"=>25,"sm"=>437,"stm"=>13,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>28,"bh"=>11,"bm"=>24,"ah"=>4,"am"=>28,"sm"=>251,"stm"=>5,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
{"wd"=>29,"bh"=>10,"bm"=>51,"ah"=>6,"am"=>54,"sm"=>463,"stm"=>7,"a"=>[{"ah"=>3,"am"=>0,"at"=>10}] },
]

values=[
258.00,
258.00,
255.00,
255.00,
254.00,
258.00,
255.00,
253.00,
253.00,
257.00,
]

(0..weight.length).each{ |i| puts "<thing><type-id name='Weight Measurement'>3d34d87e-7fc1-4153-800f-f56592cb0d17</type-id><flags>0</flags><data-xml><weight><when><date><y>2011</y><m>06</m><d>"+weight[i]["wd"].to_s+"</d></date><time><h>00</h><m>0</m><s>0</s></time></when><value><kg>"+(0.4535*values[i]).to_s+"</kg><display units-code='lb' units='pounds'>"+values[i]+"</display></weight><common><source>Withings</source></common></data-xml></thing>"

}
