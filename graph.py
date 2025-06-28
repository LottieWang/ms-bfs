import os

website="https://pasgal-bs.cs.ucr.edu/bin/"

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
# GRAPH_DIR = f"{CURRENT_DIR}/../data/graphs"
GRAPH_DIR = f"/data/graphs/bin"
# TR_DIR = f"/data/lwang323/Approx/ground_truth"

graphs = [
"Epinions1_sym",
"Slashdot_sym",
"DBLP_sym",
"com-youtube_sym",
"skitter_sym",
"in_2004_sym",
"soc-LiveJournal1_sym",
"hollywood_2009_sym",
"socfb-uci-uni_sym",
"socfb-konect_sym",
"com-orkut_sym",
"indochina_sym",
"eu-2015-host_sym",
"uk-2002_sym",
"arabic_sym",
"twitter_sym",
"friendster_sym",
"sd_arc_sym"
]

largeD_graphs=[
    "RoadUSA_sym",
    "Germany_sym",
    "africa_sym",
    "asia_sym",
    "HT_5_sym",
    "Household_5_sym",
    "CHEM_5_sym",
    "GeoLifeNoScale_5_sym",
    "Cosmo50_5_sym",
]

dir_graphs=[
    "soc-LiveJournal1",
    "twitter",
    "sd_arc",
]
dir_largeD_graphs=[
    "clueweb",
    "HT_5",
    "Household_5",
    "CHEM_5",
    "GeoLifeNoScale_5",
    "Cosmo50_5",
]

graph_map={
"Epinions1": "EP",
"Slashdot":"SLDT",
"DBLP":"DBLP",
"com-youtube":"YT",
"skitter":"SK",
"in_2004":"IN04",
"soc-LiveJournal1":"LJ",
"hollywood_2009":"HW",
"socfb-uci-uni":"FBUU",
"socfb-konect":"FBKN",
"com-orkut":"OK",
"indochina":"INDO",
"eu-2015-host":"EU",
"uk-2002":"UK",
"arabic":"AR",
"twitter":"TW",
"friendster":"FT",
"sd_arc":"SD"
}