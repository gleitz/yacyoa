function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}
COUNTER=123 # start of the custom tracks
TRACKS_ARR=();
> songs.txt

trash songs-converted
mkdir songs-converted

cd songs
for file in *.mp3;

do
   PADDED_COUNTER=$(printf "%03d" $COUNTER);
   TRACK_NAME="TRACK_$(echo ${file%.*} | iconv -c -t ascii//TRANSLIT | sed -r s/[^a-zA-Z0-9]+/_/g | sed -r s/^-+\|-+$//g | tr a-z A-Z | sed s/_$//)";
   TRACKS_ARR+=(${TRACK_NAME});
   echo "const char ${TRACK_NAME}[] PROGMEM = \"${PADDED_COUNTER}\";" | tee -a ../songs.txt;
   FILENAME="track${PADDED_COUNTER}.mp3";
   sox "$file" --norm=-0.1 -C 192 -c 1 "../songs-converted/$FILENAME" -V;
   ((COUNTER++));
done
echo "const char* const RANDOM_TRACKS[] PROGMEM = { $(join_by ", " "${TRACKS_ARR[@]}") };" | tee -a ../songs.txt

cd ..
